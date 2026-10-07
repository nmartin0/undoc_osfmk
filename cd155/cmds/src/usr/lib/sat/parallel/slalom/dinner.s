// dinner (n,a,b,c)     fortran callable
//
// double precision inner product c = sum of all a(i)*b(i) 
// for i = 1 to  n
// 
// This routine performs a dot product on two double precision 
// vectors of length n.
// If n is larger than cache strip mining should be performed in the calling
// routine to avoid thrashing.
// 
// Registers loaded by calling routine:
//
// r16     pointer to n
// r17     pointer to a
// r18     pointer to b
// r19     pointer to c
//
// Registers used internally
//
// r20 = n/4  since each inner loop iteration does four multiplies
// and four adds.
// r21	[n/4] * 4
// r23 = n
// r30 = loop counter
// r31 = loop increment
//
// f16 = a1
// f18 = a2
// f20 = a3
// f22 = a4
// f24 = b1
// f26 = b2
// f28 = b3
// f30 = b4
//
_dinner_::
.align 8

fmov.dd f0, f20			// put 0.d0 in f20
ld.l 0(r16), r23		// read n

adds -1, r0, r31   	        // set loop increment to -1

// check for n < 13
adds -13, r23, r0
bc .n_lt_13

// quad-alignment check
and 15, r17, r0		// a is quad_aligned??
bc.t .quad_aligned

// adjust pointer for b (misaligned case only)

adds -8, r18, r18      // vector b ready for pipelined loads

fld.d 0(r17), f16	// load a1
adds 8, r17, r17	// misaligned vector a ready for quad loads
fld.d 0(r18), f24	// load b1
fmul.dd f16, f24, f20	// put a1*b1 in f20
adds -1, r23, r23	// subtract 1 from n

.quad_aligned:

// find n/4
shr 2, r23, r20

// set loop parameters
adds -3, r20, r30         // set loop counter to n/4-3 (-2 for
//                             tailout, -1 for bla)

.align 8
d.pfadd.dd f20, f0, f0	// put 0.d0(aligned) or a1*b1(misaligned) into add pipe
  bla r31, r30, dummy     // initialize lc for inner loop
d.fnop
  adds -16, r17, r17     // vector a ready for quad loads

//                         LOAD PIPE  MULT PIPE  ADD PIPE
dummy:
d.fnop
  pfld.d 8(r18)++, f0      // b1,? ,?     ? ,?     0 ,? ,?
d.pfmul.dd f0, f0, f0     // b1,? ,?     0 ,?     0 ,? ,?
  fld.q 16(r17)++, f16     // a1 and a2 loaded
d.pfadd.dd f0, f0, f0
  pfld.d 8(r18)++, f0      // b2,b1,?     0 ,?     0 ,0 ,?
d.pfmul.dd f0, f0, f0
  shl 2, r20, r21          // b2,b1,?     0 ,0     0 ,0 ,?
d.pfadd.dd f0, f0, f0
  pfld.d 8(r18)++, f0      // b3,b2,b1    0 ,0     0 ,0 ,0

// get b1 and b2 out of load pipe into registers

d.fnop
  pfld.d 8(r18)++, f24     // b4,b3,b2    0 ,0     0 ,0 ,0
d.fnop
  pfld.d 8(r18)++, f26     // b5,b4,b3    0 ,0     0 ,0 ,0

// start the inner loop processing sets of four elements

loop:     // loop for each set of four elements of a and b

//                           pipe values first time through loop
//                            LOADING    MULT      ADDER
d.m12apm.dd f16, f24, f0 //   b5,b4,b3    p1,0     0 ,0 ,0
  fld.q 16(r17)++, f20   // load a3 and a4 into f20 and f22
d.fnop                   // then load b3 into register f28
  pfld.d 8(r18)++, f28   //   b6,b5,b4    p1,0     0 ,0 ,0
d.m12apm.dd f18, f26, f0 //   b6,b5,b4    p2,p1    0 ,0 ,0
  nop
d.fnop                   //   then load b4 into f30
  pfld.d 8(r18)++, f30   //   b7,b6,b5    p2,p1    0 ,0 ,0
d.m12apm.dd f20, f28, f0 //   b7,b6,b5    p3,p2    p1,0 ,0
  fld.q 16(r17)++, f16   // load a1 and a2 for next loop f16, f18
d.fnop                   // then get b5 (b1 for next loop) into f24
  pfld.d 8(r18)++, f24   //   b8,b7,b6    p3,p2    p1,0 ,0
d.m12apm.dd f22, f30, f0 //   b8,b7,b6    p4,p3    p2,p1,0
  bla r31, r30, loop
d.fnop                   // then get b6 (b2 for next loop) into f26
  pfld.d 8(r18)++, f26   //   b9,b8,b7    p4,p3    p2,p1,0

// do last 2 iterations outside loop to avoid reading past vectors

d.m12apm.dd f16, f24, f0 // a1*b1 into mult pipe
  fld.q 16(r17)++, f20   // load values of a3 and a4 into f20, f22
d.fnop
  pfld.d 8(r18)++, f28   // load b3 into f28
d.m12apm.dd f18, f26, f0 // a2*b2 into mult pipe
  nop
d.fnop
  pfld.d 8(r18)++, f30   // load b4 into f30
d.m12apm.dd f20, f28, f0 // a3*b3 into mult pipe
  fld.q 16(r17)++, f16   // load a1 and a2 into f16, f18
d.fnop
  pfld.d 8(r18)++, f24   // load b1 -> f24, done loading pfld pipe
d.m12apm.dd f22, f30, f0// a4*b4 into mult pipe
  nop

//    start flushing load pipeline (3 stages)

d.fnop
  pfld.d 0(r18), f26   // load b2 -> f26, push junk into load pipe

// last 'loop' using real values in mult pipeline

d.m12apm.dd f16, f24, f0 // a1*b1
  fld.q 16(r17)++, f20    // load values of a3 and a4 into f20, f22
d.fnop
  pfld.d 0(r18), f28         // load b3 into f28, 2nd junk into load pipe
d.m12apm.dd f18, f26, f0 // a2*b2 into mult pipe
  adds 8, r18, r18
d.fnop
  pfld.d 0(r18), f30         // load b4 -> f30, load pipe now all junk
d.m12apm.dd f20, f28, f0 // a3*b3 into mult pipe
  nop
d.m12apm.dd f22, f30, f0 // a4*b4 into mult pipe
  adds 16, r17, r17

//  start flushing multiply pipeline (2 stages)

m12apm.dd f0, f0, f0   //0 into mult pipe, last p3 -> adder
  nop
m12apm.dd f0, f0, f0 //0-> mult pipe, last p4 into add pipe
  subs r23, r21, r23     //mult pipe is all zeros

// flush adder pipeline, combining partial sums of p1,2,3,4
//
//                        adder pipeline:   f18: f20: f22: 
pfadd.dd f0, f0, f18   // 0 p14 p3          p2    -    -
pfadd.dd f0, f0, f20   // 0  0  p14         p2   p3    -
pfadd.dd f18, f20, f22 // p23 0 0           p2   p3   p14
pfadd.dd f0, f0, f0    // 0  p23 0
pfadd.dd f0, f0, f0    // 0  0  p23  
pfadd.dd f0, f0, f18   // 0  0  0           p23  -    p14
fadd.dd  f18, f22, f20  // final sum, c, into f20

.n_lt_13:

adds -1, r23, r30		// set loop counter to n - 1
bc .n_lt_1			// check for n < 1
adds -8, r18, r18
bla r31, r30, sim_loop	// initialize lc for simple loop
adds -8, r17, r17
sim_loop:
fld.d 8(r18)++, f24
fld.d 8(r17)++, f16
fmul.dd f16, f24, f26
bla r31, r30, sim_loop
fadd.dd f26, f20, f20
.n_lt_1:
bri r1
fst.d f20, 0(r19)

