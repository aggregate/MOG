/*	mog.cu

	MIMD On GPU simulator by Hank Dietz

	This simulator uses an accumulator-based instruction set that
	is based on reprocessing MIPSEL assembly code.  The instruction
	set is fundamentally similar, but has been simplified to make
	decoding instructions trivial.

	20101122 first alpha test release
*/

#define	VERSION	20101122
#define	TARGET	"cuda"

#define	TRACE
#undef	TRACE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <cutil.h>
#include <helper_cuda.h>
#include <helper_timer.h>

#include "op.h"
#include "do.h"
#include "pr.h"

#include "mog.h"

#define	GPU_MULTIPROCESSORS	4
#define	GPU_WARPSIZE		32
#define	GPU_THREADSPERBLOCK	(8*GPU_WARPSIZE)

#define	SHARED_SKEW	16

#define	WARPSIZE	32		/* Size of a WARP on this GPU */
#define	WARPDIV(X)	((X)>>5)	/* X / WARPSIZE */
#define	WARPMOD(X)	((X)&31)	/* X % WARPSIZE */
#define	WARPMUL(X)	((X)<<5)	/* X * WARPSIZE */

#define	MAR(X)		(((X)&~3)<<3)	/* X * WARPSIZE / 4 */

/*	TIMEOUT can be anything; 1 for TRACE */
#ifdef	TRACE
#define	TIMEOUT		1
#else
#define	TIMEOUT		(1024)
#endif

#define	BNPROC		GPU_THREADSPERBLOCK
#define	NPROC		(GPU_MULTIPROCESSORS*BNPROC)

typedef union word_union {
	float			f;
	int			i;
	unsigned int		u;
	short			h[2];
	signed char		b[4];
} word_t;

#define	FLAGS	1		/* Number of different flag states */

typedef struct flags_struct {
	int	flag[FLAGS];	/* Flags showing which inst present */
	int isSyscall[NPROC];
} flags_t;

#define NSYSARGS_host 20

typedef struct sysargs_struct {
	word_t sysarg[NPROC][NSYSARGS_host];
} arg_t;

typedef struct pe_struct {
	word_t	regs[REGSUSED];	/* Copy of regs for this PE */
	word_t	a;		/* Copy of accumulator */
	int	pc;		/* PC offset */
	int	ir;		/* IR */
} pe_t;

typedef struct data_struct {
	flags_t	flags;		/* Emulator flags */
	pe_t	pe[NPROC];	/* PE state info (fixed size) */
				/* Memory pool (potentially variable size) */
	word_t	mem[WARPDIV(NPROC)][DATASIZE][WARPSIZE];
} data_t;

typedef	struct {
	word_t	regs[WARPDIV(BNPROC)][REGSUSED][WARPSIZE];
} my_shared_t;

texture<unsigned short, 1, cudaReadModeElementType> codetex;
texture<int, 1, cudaReadModeElementType> cpooltex;

/*	Emulator as a single fragment function
*/
template <unsigned int blockSize>
__global__ void
emulate(register data_t *alldata, arg_t *hostsysargs)
{
	extern __shared__ my_shared_t	myshared[];
#define	BIPROC			(threadIdx.x)
	register int		warpmodbiproc = WARPMOD(BIPROC);
	register int		warpdivbiproc = WARPDIV(BIPROC);
	register const int	IPROC = BIPROC + (blockIdx.x * blockSize);
	register unsigned int	ir, op;
	register int		pc = alldata->pe[IPROC].pc;
	register word_t		a;
	register int		serial = 0;
	register volatile word_t	*mem = &(alldata->mem[WARPDIV(IPROC)][0][WARPMOD(BIPROC)]);
	register word_t		*regs = ((word_t *) &((*myshared).regs[warpdivbiproc][0][warpmodbiproc]));
	register int		moretodo;

#define	REGF(N)		regs[WARPMUL(REGNAME(N))].f
#define	REGI(N)		regs[WARPMUL(REGNAME(N))].i
#define	REGU(N)		regs[WARPMUL(REGNAME(N))].u

#define	MEMI(M)		mem[MAR(M)].i
#define	MEMH(M)		mem[MAR(M)].h[((M)&2)>>1]
#define	MEMB(M)		mem[MAR(M)].b[(M)&3]

	/*convert argBuf to GPU memory structure*/
	for(a.i=0; a.i<(sizeof(word_t)*MEMI(MOGSYM_NSYSARGS)); a.i=a.i+sizeof(word_t))
	 	MEMI(MOGSYM_SYSARGS+a.i) = hostsysargs->sysarg[IPROC][a.i/sizeof(word_t)].i;

	/* Reset flags */
	/*only one for now*/
	if (IPROC < FLAGS) alldata->flags.flag[IPROC] = 0;

	/* Restore registers */
	for (a.i=0; a.i<REGSUSED; ++a.i) {
		REGI(a.i) = alldata->pe[IPROC].regs[a.i].i;
	}
	a = alldata->pe[IPROC].a;

#define	CPOOL(p, i)	tex1D(cpooltex, CHASH(p-1, i))

#define	CODE(addr)	tex1D(codetex, addr)

//define	NEXT		{ op = OPCODE(ir = CODE(pc++)); }
#define	NEXT		{ ir = CODE(pc); op = OPCODE(ir); ++pc; }

#define	OPIS(o)		if (op == (OP##o)) { DO##o }

	/* Fetch current instruction */
	NEXT;

	do {
		/* Decode and execute single-instruction interpreters */
		moretodo = (TIMEOUT + 1);

		/* Expand-out the optimized interpreter sequence */
		OPORDER

		++serial;
		if (op != OPsys) moretodo = serial;
	} while (moretodo < TIMEOUT);

	__syncthreads();

	/* Is everybody stuck at a syscall? */
	if (op != OPsys) {
		/* Nope. */
		//alldata->flags.flag[0] = 0;
		alldata->flags.isSyscall[IPROC] = 0;
	}
	else { /*convert sysargs back to host format*/
		hostsysargs->sysarg[IPROC][0].i = MEMI(MOGSYM_NSYSARGS);
		for(a.i=0; a.i<(sizeof(word_t)*(MEMI(MOGSYM_NSYSARGS))); a.i=a.i+sizeof(word_t))
        	memcpy((void *)&hostsysargs->sysarg[IPROC][(a.i/sizeof(word_t))+1], (void *)&mem[MAR(MOGSYM_SYSARGS+a.i)],4);
		++pc;
		//alldata->flags.flag[0] = 1;
		alldata->flags.isSyscall[IPROC] = 1;
	}

	__syncthreads();

	/* Save registers */
	alldata->pe[IPROC].a = a;
	alldata->pe[IPROC].pc = pc - 1;
	for (a.i=0; a.i<REGSUSED; ++a.i) {
		alldata->pe[IPROC].regs[a.i].i = REGI(a.i);
	}
}

/*	Trace emulator as a single fragment function
*/
template <unsigned int blockSize>
__global__ void
emutrace(register data_t *alldata)
{
	extern __shared__ my_shared_t	myshared[];
#define	BIPROC			(threadIdx.x)
	register int		warpmodbiproc = WARPMOD(BIPROC);
	register int		warpdivbiproc = WARPDIV(BIPROC);
	register const int	IPROC = BIPROC + (blockIdx.x * blockSize);
	register unsigned int	ir, op;
	register int		pc = alldata->pe[IPROC].pc;
	register word_t	a;
	register int		serial = 0;
	register volatile word_t	*mem = &(alldata->mem[WARPDIV(IPROC)][0][WARPMOD(BIPROC)]);
	register word_t	*regs = ((word_t *) &((*myshared).regs[warpdivbiproc][0][warpmodbiproc]));
	register int		moretodo;

#define	REGF(N)		regs[WARPMUL(REGNAME(N))].f
#define	REGI(N)		regs[WARPMUL(REGNAME(N))].i
#define	REGU(N)		regs[WARPMUL(REGNAME(N))].u

#define	MEMI(M)		mem[MAR(M)].i
#define	MEMH(M)		mem[MAR(M)].h[((M)&2)>>1]
#define	MEMB(M)		mem[MAR(M)].b[(M)&3]


	/* Reset flags */
	if (IPROC < FLAGS) alldata->flags.flag[IPROC] = 0;

	/* Restore registers */
	for (a.i=0; a.i<REGSUSED; ++a.i) {
//	for (a.i=REGSUSED-1; a.i>=0; --a.i) {
		REGI(a.i) = alldata->pe[IPROC].regs[a.i].i;
	}
	a = alldata->pe[IPROC].a;


#define	CPOOL(p, i)	tex1D(cpooltex, CHASH(p-1, i))

#define	CODE(addr)	tex1D(codetex, addr)

//define	NEXT		{ op = OPCODE(ir = CODE(pc++)); }
#define	NEXT		{ ir = CODE(pc); op = OPCODE(ir); ++pc; }

#define	OPIS(o)		if (op == (OP##o)) { DO##o }

	/* Fetch current instruction */
	NEXT;

	do {
		/* Decode and execute single-instruction interpreters */
		moretodo = (TIMEOUT + 1);

#ifdef	TRACE
#define	TRACEELSE	else
#else
#define	TRACEELSE	/* don't serialize */
#endif

		OPIS(add)
TRACEELSE	OPIS(addf)
TRACEELSE	OPIS(and)
TRACEELSE	OPIS(div)
TRACEELSE	OPIS(divu)
TRACEELSE	OPIS(divf)
TRACEELSE	OPIS(xor)
TRACEELSE	OPIS(mul)
TRACEELSE	OPIS(mulf)
TRACEELSE	OPIS(or)
TRACEELSE	OPIS(slt)
TRACEELSE	OPIS(sltu)
TRACEELSE	OPIS(sltf)
TRACEELSE	OPIS(neg)
TRACEELSE	OPIS(negf)
TRACEELSE	OPIS(rem)
TRACEELSE	OPIS(remu)
TRACEELSE	OPIS(sra)
TRACEELSE	OPIS(sll)
TRACEELSE	OPIS(srl)

TRACEELSE	OPIS(j)
TRACEELSE	OPIS(jf)
TRACEELSE	OPIS(jt)

TRACEELSE	OPIS(i2f)
TRACEELSE	OPIS(u2f)
TRACEELSE	OPIS(f2i)

TRACEELSE	OPIS(li)
TRACEELSE	OPIS(lr)
TRACEELSE	OPIS(sr)
TRACEELSE	OPIS(lw)
TRACEELSE	OPIS(sw)
TRACEELSE	OPIS(lh)
TRACEELSE	OPIS(sh)
TRACEELSE	OPIS(lb)
TRACEELSE	OPIS(sb)

		++serial;
		if (op != OPsys) moretodo = serial;
	} while (moretodo < TIMEOUT);

	__syncthreads();

	/* Is everybody stuck at a syscall? */
	if (op != OPsys) {
		/* Nope. */
		alldata->flags.flag[0] = 1;
	}
	__syncthreads();

	/* Save registers */
	alldata->pe[IPROC].a = a;
	alldata->pe[IPROC].pc = pc - 1;
	for (a.i=0; a.i<REGSUSED; ++a.i) {
		alldata->pe[IPROC].regs[a.i].i = REGI(a.i);
	}
}

data_t	alldata;
arg_t hostsysargs;

/*	Status output for PE IPROC
*/
void
status(register const int IPROC)
{
	register int pc = alldata.pe[IPROC].pc;
	register int a = alldata.pe[IPROC].a.i;
	register int ir = textseg[pc];
	register int op = OPCODE(ir);
	register int i;

#undef	CPOOL
#define	CPOOL(PC,IR)	cpool[CHASH((PC), (IR))]
#define	IRIS(o)		{ if (op == (OP##o)) PR##o; }

	/* Dump registers */
	printf("PE%d: ", IPROC);
	printf("pc=0x%x", pc);
	printf(" ir=%c", '"');

		IRIS(sys)

		IRIS(add)
		IRIS(addf)
		IRIS(and)
		IRIS(div)
		IRIS(divu)
		IRIS(divf)
		IRIS(xor)
		IRIS(mul)
		IRIS(mulf)
		IRIS(or)
		IRIS(slt)
		IRIS(sltu)
		IRIS(sltf)
		IRIS(neg)
		IRIS(negf)
		IRIS(rem)
		IRIS(remu)
		IRIS(sra)
		IRIS(sll)
		IRIS(srl)

		IRIS(j)
		IRIS(jf)
		IRIS(jt)

		IRIS(i2f)
		IRIS(u2f)
		IRIS(f2i)

		IRIS(li)
		IRIS(lr)
		IRIS(sr)
		IRIS(lw)
		IRIS(sw)
		IRIS(lh)
		IRIS(sh)
		IRIS(lb)
		IRIS(sb)

	printf("%c a=0x%x", '"', a);
	for (i=0; i<REGSUSED; ++i) {
		printf(" $%d=%d",
		       i,
		       alldata.pe[IPROC].regs[i].i);
	}
	printf("\n");
}

int alldone(int* done)
{
	int total = 0;
	for(int i=0; i<NPROC; ++i)
		total += done[i];

	//printf("total: %i\n",total);
	if(total==NPROC)
		return 1;
	else 
		return 0;
}

int
main(int argc, char **argv) 
{
	register int i, j;
	int done[NPROC] = {0}; //used to determine exit condition
	data_t *gpudata;
	arg_t *gpusysargs;
	dim3 dimBlock(BNPROC, 1, 1);
	dim3 dimGrid((NPROC/BNPROC), 1, 1);

	/* Variables for a cuda timer */
	StopWatchInterface *timer = NULL;
	float exeTime = 0.0f;

	/* Create and reset a timer to measure the speed */
	sdkCreateTimer(&timer);
	sdkResetTimer(&timer);

	/* Check version and target */
	if (VERSION != version) {
		fprintf(stderr, "Error: code was compiled for version %d, not %d\n", version, VERSION);
		exit(1);
	}
	if (strcmp(TARGET, target) != 0) {
		fprintf(stderr, "Error: code was compiled for target %s, not %s\n", target, TARGET);
		exit(1);
	}

	/* Initialize everything... */
	for (i=0; i<NPROC; ++i) {
		/* Fill data memory, setting NPROC and IPROC */
		alldata.mem[WARPDIV(i)][0][WARPMOD(i)].i = NPROC;
		alldata.mem[WARPDIV(i)][1][WARPMOD(i)].i = i;
		for (j=2; j<DATASIZE; ++j) {
			alldata.mem[WARPDIV(i)][j][WARPMOD(i)].i = dataseg[j];
		}

		/* Initialize sp, ra, and other registers */
		for (j=2; j<REGSUSED; ++j) {
			alldata.pe[i].regs[j].i = 42;
		}
		alldata.pe[i].regs[0].i = DATASIZE-1;

		/* Set-up PE status regs */
		alldata.pe[i].pc = MOGSYM_main;
		alldata.pe[i].a.i = 0;

		/*initialize argbuf to nul(-1)*/
		for(j=0; j<NSYSARGS_host; j++)
			hostsysargs.sysarg[i][j].i=-1;
	}

		/* Allocate and copy alldata to GPU memory */
		checkCudaErrors( cudaMalloc((void**) &gpudata, sizeof(data_t)) );
		checkCudaErrors( cudaMemcpy(gpudata, &(alldata), sizeof(data_t), cudaMemcpyHostToDevice) );

		/* Allocate and copy hostsysargs to GPU memory */
		checkCudaErrors( cudaMalloc((void**) &gpusysargs, sizeof(arg_t)) );
		checkCudaErrors( cudaMemcpy(gpusysargs, &(hostsysargs), sizeof(arg_t), cudaMemcpyHostToDevice) );

	/* Allocate and copy cpool[] to GPU texture */
	cudaChannelFormatDesc channelDesc2 = cudaCreateChannelDesc<int>();
	cudaArray* gputext2;
	checkCudaErrors(cudaMallocArray( &gputext2, &channelDesc2, sizeof(cpool)/sizeof(int), 1));
	checkCudaErrors(cudaMemcpyToArray( gputext2, 0, 0, cpool, sizeof(cpool), cudaMemcpyHostToDevice));

	cpooltex.filterMode = cudaFilterModePoint;
	cpooltex.normalized = false;    // access with unnormalized texture coordinates
	cpooltex.addressMode[0] = cudaAddressModeClamp;   // wrap texture coordinates

	checkCudaErrors( cudaBindTextureToArray( cpooltex, gputext2, channelDesc2));


	/* Allocate and copy textseg[] to GPU texture */
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<unsigned short>();
	cudaArray* gputext;
	checkCudaErrors(cudaMallocArray( &gputext, &channelDesc, sizeof(textseg)/sizeof(unsigned short), 1));
	checkCudaErrors(cudaMemcpyToArray( gputext, 0, 0, textseg, sizeof(textseg), cudaMemcpyHostToDevice));

	codetex.filterMode = cudaFilterModePoint;
	codetex.normalized = false;    // access with unnormalized texture coordinates
	codetex.addressMode[0] = cudaAddressModeClamp;   // wrap texture coordinates

	checkCudaErrors( cudaBindTextureToArray( codetex, gputext, channelDesc));

	/* Time the main program execution */
	sdkStartTimer(&timer);

#ifdef	TRACE
	printf("flag = %d\n", alldata.flags.flag[0]);
	for (i=0; i<NPROC; ++i) {
		status(i);
	}
	do {
		/* Run the emulator for one instruction */
		emutrace<BNPROC><<< dimGrid, dimBlock, sizeof(my_shared_t) >>>(gpudata);

		/* Copy stuff back to host...
		   If we are doing a debug trace, copy entire data,
		   else could copy only the flags
		*/
		checkCudaErrors( cudaMemcpy(&(alldata), gpudata, sizeof(data_t), cudaMemcpyDeviceToHost) );
		printf("flag = %d\n", alldata.flags.flag[0]);
		for (i=0; i<NPROC; ++i) {
			status(i);
		}
	} while (alldata.flags.flag[0] != 0);
#else
	do {
		/* Run the emulator for a while */
		emulate<BNPROC><<< dimGrid, dimBlock, sizeof(my_shared_t) >>>(gpudata, gpusysargs);

		/* Copy stuff back to host...
		   If we are doing a debug trace, copy entire data,
		   else could copy only the flags
		*/
		checkCudaErrors( cudaMemcpy(&(alldata), gpudata, sizeof(flags_t), cudaMemcpyDeviceToHost) );

		//test
		checkCudaErrors( cudaMemcpy(&(hostsysargs), gpusysargs, sizeof(arg_t), cudaMemcpyDeviceToHost) );

		//if(alldata.flags.flag[0] == 1) {/*if there was a syscall start decoding*/
		/*copy sysargs back to host*/
		for(i=0; i<NPROC; i++)	{
			int nsysargs = hostsysargs.sysarg[i][0].i;
			int sysCallNum = hostsysargs.sysarg[i][1].i;
			/*look in argument buffers, decode system call, execute it within some environment, then return data*/
			if(alldata.flags.isSyscall[i] == 1) {
				switch(sysCallNum) {
					case 0: {/*exit()*/
						done[i]=1;
					}
					break;
					case 1:	{/*time(time_t *t)*/
						hostsysargs.sysarg[i][0].i = time(NULL);
						printf("time: %i\n", hostsysargs.sysarg[i][2].i);
					}
					break;
					case 2: {/*dup(int filedes)*/
						int filedes=hostsysargs.sysarg[i][2].i;
	                    printf("filedes: %i\n",filedes);
						hostsysargs.sysarg[i][0].i = filedes;
					}
					break;
					case 3: {/*putchar(char c)*/
						char c=(char)hostsysargs.sysarg[i][2].i;
						putchar(c);
					}
					break;
					case 4: {/*getchar()*/
						hostsysargs.sysarg[i][0].i = getchar();
					}
					break;
					case 5: {/*open(const char *pathname, int flags)*/
						char *pathname = (char *)malloc((nsysargs-3)*sizeof(char));
						int ichar;
						for(ichar=0; ichar<(nsysargs-3); ++ichar)
							pathname[ichar] = (char)hostsysargs.sysarg[i][ichar+2].i;
						int fileflags = hostsysargs.sysarg[i][ichar+2].i;
						int filemode = hostsysargs.sysarg[i][ichar+3].i;
						printf("pathname: %s\nfileflags: %i\nfilemode: %i\n",pathname,fileflags,filemode);
						int ro = open(pathname,fileflags,filemode);
						hostsysargs.sysarg[i][0].i = ro;
						printf("openfd: %i\n",ro);
					}
					break;
					case 6: {/*close(int fd)*/
						int fd = hostsysargs.sysarg[i][2].i;
						close(fd);
					}
					break;
					case 7: {/*int read(int fd, void *buf, int count)*/
						int fd = hostsysargs.sysarg[i][2].i;
						int count = hostsysargs.sysarg[i][3].i;
						read(fd,(void *)(&hostsysargs.sysarg[i][0]),count);
						printf("read: count: %i fd: %i buf: %s\n",count,fd,(char *)(&hostsysargs.sysarg[i][0]));
            
					}
					break;
					case 8: {/*int write(int fd, const void *buf, int count)*/ 
						int fd = hostsysargs.sysarg[i][2].i;
						int count = hostsysargs.sysarg[i][3].i;
						char buf[count];
						memcpy((void *)buf,(void *)(&hostsysargs.sysarg[i][4]),count);
						printf("write: count: %i fd: %i buf: %s other: %i %i %i\n",count,fd,buf,buf[0],buf[1],buf[2]);
						write(fd,buf,count);
					}
					break;
					case 9: {/*putint(int i)*/
						int anint=hostsysargs.sysarg[i][2].i;
						printf("%i\n",anint);
					}
					break;
					default:
					break;
				}
			}
		}
		checkCudaErrors( cudaMemcpy(gpusysargs, &(hostsysargs), sizeof(arg_t), cudaMemcpyHostToDevice) );
	} while (!alldone(done));
#endif

	checkCudaErrors( cudaMemcpy(&(alldata), gpudata, sizeof(data_t), cudaMemcpyDeviceToHost) );

	/* Stop the timer */
	sdkStopTimer(&timer);

	/* Print the execution time of the mulate function (and it's loop/readback) */
	exeTime = sdkGetTimerValue(&timer);
	printf("\nTotal emulate time (without setup): %.3f s\n",exeTime/1000.0f);

        checkCudaErrors(cudaFree(gpudata));
	sdkDeleteTimer(&timer);
}
