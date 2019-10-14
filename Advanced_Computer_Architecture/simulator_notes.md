# Table of Contents

1.  [About](#orgefb9a7d)
    1.  [Register Table](#orgf17d9d5)
    2.  [Instruction Set](#org39d2253)
2.  [Test Programs](#org47d121f)
    1.  [Livermore Loops:](#orgb156fe1)



<a id="orgefb9a7d"></a>

# About

The Simulator is modelled off the MIPS ISA. As such it has a similar
Instruction Set which is described below.


<a id="orgf17d9d5"></a>

## Register Table

Continuing to model off the MIPS ISA, the register table is the same
as MIPS which is described below:

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="org-left" />

<col  class="org-left" />

<col  class="org-left" />
</colgroup>
<thead>
<tr>
<th scope="col" class="org-left">Register Number</th>
<th scope="col" class="org-left">Conventional Name</th>
<th scope="col" class="org-left">Usage</th>
</tr>
</thead>

<tbody>
<tr>
<td class="org-left">$0</td>
<td class="org-left">$zero</td>
<td class="org-left">Hardwired to 0</td>
</tr>


<tr>
<td class="org-left">$1</td>
<td class="org-left">$at</td>
<td class="org-left">Reserved for pseudo-instructions</td>
</tr>


<tr>
<td class="org-left">$2-$3</td>
<td class="org-left">$v0,$v1</td>
<td class="org-left">Return Values from functions</td>
</tr>


<tr>
<td class="org-left">$4-$7</td>
<td class="org-left">$a0-$a3</td>
<td class="org-left">Arguments to functions - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$8-$15</td>
<td class="org-left">$t0-$t7</td>
<td class="org-left">Temporary Data - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$16-$23</td>
<td class="org-left">$s0-$s7</td>
<td class="org-left">Saved Registers - preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$24-$25</td>
<td class="org-left">$t8-$t9</td>
<td class="org-left">More temporary registers - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$26-$27</td>
<td class="org-left">$k0 - $k1</td>
<td class="org-left">Reserved for kernel. DO NOT USE</td>
</tr>


<tr>
<td class="org-left">$28</td>
<td class="org-left">$gp</td>
<td class="org-left">Global Area Pointer (base of global data segment)</td>
</tr>


<tr>
<td class="org-left">$29</td>
<td class="org-left">$sp</td>
<td class="org-left">Stack Pointer</td>
</tr>


<tr>
<td class="org-left">$30</td>
<td class="org-left">$fp</td>
<td class="org-left">Frame Pointer</td>
</tr>


<tr>
<td class="org-left">$31</td>
<td class="org-left">$ra</td>
<td class="org-left">Return Address</td>
</tr>


<tr>
<td class="org-left">$f0-$f3</td>
<td class="org-left">&#xa0;</td>
<td class="org-left">Floating Point Return values</td>
</tr>


<tr>
<td class="org-left">$f4-$f10</td>
<td class="org-left">&#xa0;</td>
<td class="org-left">Temporary Registers - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$f12-$f14</td>
<td class="org-left">&#xa0;</td>
<td class="org-left">First two arguments to subprograms - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$f16-$f18</td>
<td class="org-left">&#xa0;</td>
<td class="org-left">More temporary registers - not preserved by subprograms</td>
</tr>


<tr>
<td class="org-left">$f20-$f30</td>
<td class="org-left">&#xa0;</td>
<td class="org-left">Saved registers - preserved by subprograms</td>
</tr>
</tbody>
</table>


<a id="org39d2253"></a>

## Instruction Set

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="org-left" />

<col  class="org-left" />
</colgroup>
<thead>
<tr>
<th scope="col" class="org-left">Instruction</th>
<th scope="col" class="org-left">Description</th>
</tr>
</thead>

<tbody>
<tr>
<td class="org-left">ADD</td>
<td class="org-left">Add (w/Overflow)</td>
</tr>


<tr>
<td class="org-left">ADDI</td>
<td class="org-left">Add immediate (w/Overflow)</td>
</tr>


<tr>
<td class="org-left">ADDIU</td>
<td class="org-left">Add immediate unsigned (wo/Overflow)</td>
</tr>


<tr>
<td class="org-left">ADDU</td>
<td class="org-left">Add unsigned (no Overflow)</td>
</tr>


<tr>
<td class="org-left">AND</td>
<td class="org-left">Bitwise AND</td>
</tr>


<tr>
<td class="org-left">ANDI</td>
<td class="org-left">Bitwise AND immediate</td>
</tr>


<tr>
<td class="org-left">BEQ</td>
<td class="org-left">Branch on Equal</td>
</tr>


<tr>
<td class="org-left">BGEZ</td>
<td class="org-left">Branch on greater than or equal to zero</td>
</tr>


<tr>
<td class="org-left">BGEZAL</td>
<td class="org-left">Branch on greater than or equal to zero and link</td>
</tr>


<tr>
<td class="org-left">BGTZ</td>
<td class="org-left">Branch on greater than zero</td>
</tr>


<tr>
<td class="org-left">BLEZ</td>
<td class="org-left">Branch on less than or equal to zero</td>
</tr>


<tr>
<td class="org-left">BLTZ</td>
<td class="org-left">Branch on less than zero</td>
</tr>


<tr>
<td class="org-left">BLTZAL</td>
<td class="org-left">Branch on less than zero and link</td>
</tr>


<tr>
<td class="org-left">BNE</td>
<td class="org-left">Branch on not equal</td>
</tr>


<tr>
<td class="org-left">DIV</td>
<td class="org-left">Divide</td>
</tr>


<tr>
<td class="org-left">DIVU</td>
<td class="org-left">Divide Unsigned</td>
</tr>


<tr>
<td class="org-left">J</td>
<td class="org-left">Unconditional Jump</td>
</tr>


<tr>
<td class="org-left">JAL</td>
<td class="org-left">Jump and link</td>
</tr>


<tr>
<td class="org-left">JR</td>
<td class="org-left">Jump to address in Register</td>
</tr>


<tr>
<td class="org-left">LB</td>
<td class="org-left">Load Byte</td>
</tr>


<tr>
<td class="org-left">LUI</td>
<td class="org-left">Load Upper Immediate</td>
</tr>


<tr>
<td class="org-left">LW</td>
<td class="org-left">Load Word</td>
</tr>


<tr>
<td class="org-left">MULT</td>
<td class="org-left">Multiply</td>
</tr>


<tr>
<td class="org-left">MULTU</td>
<td class="org-left">Multiply Unsigned</td>
</tr>


<tr>
<td class="org-left">NOOP</td>
<td class="org-left">No operation</td>
</tr>


<tr>
<td class="org-left">OR</td>
<td class="org-left">Bitwise OR</td>
</tr>


<tr>
<td class="org-left">ORI</td>
<td class="org-left">Bitwise OR Immediate</td>
</tr>


<tr>
<td class="org-left">SB</td>
<td class="org-left">Store Byte</td>
</tr>


<tr>
<td class="org-left">SLL</td>
<td class="org-left">Shift Left Logical</td>
</tr>


<tr>
<td class="org-left">SLLV</td>
<td class="org-left">Shift Left Logical Variable</td>
</tr>


<tr>
<td class="org-left">SLT</td>
<td class="org-left">Set on Less Than (signed)</td>
</tr>


<tr>
<td class="org-left">SLTI</td>
<td class="org-left">Set on Less Than Immediate (signed)</td>
</tr>


<tr>
<td class="org-left">SLTIU</td>
<td class="org-left">Set on Less Than Immediate (unsigned)</td>
</tr>


<tr>
<td class="org-left">SLTU</td>
<td class="org-left">Set on Less Than Unsigned</td>
</tr>


<tr>
<td class="org-left">SRA</td>
<td class="org-left">Shift Right Arithmetic</td>
</tr>


<tr>
<td class="org-left">SRL</td>
<td class="org-left">Shift Right Logical</td>
</tr>


<tr>
<td class="org-left">SRLV</td>
<td class="org-left">Shift Right Logical Variable</td>
</tr>


<tr>
<td class="org-left">SUB</td>
<td class="org-left">Subtract</td>
</tr>


<tr>
<td class="org-left">SUBU</td>
<td class="org-left">Subtract Unsigned</td>
</tr>


<tr>
<td class="org-left">SW</td>
<td class="org-left">Store WOrd</td>
</tr>


<tr>
<td class="org-left">SYSCALL</td>
<td class="org-left">System Call</td>
</tr>


<tr>
<td class="org-left">XOR</td>
<td class="org-left">Bitwise XOR</td>
</tr>


<tr>
<td class="org-left">XORI</td>
<td class="org-left">Bitwise XOR Immediate</td>
</tr>
</tbody>
</table>


<a id="org47d121f"></a>

# Test Programs


<a id="orgb156fe1"></a>

## Livermore Loops:

    ADD s0 s1 s2
