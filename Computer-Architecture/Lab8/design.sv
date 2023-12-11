// Code your design here
// Instruction: Modify SystemVerilog modules to implement the sw instruction.

`include "alu.sv"
`include "regfile.sv"
`include "imem.sv"
`include "dmem.sv"
`include "controller.sv"

module mips(
  input logic iClk,
  input logic iReset
);

  logic [31:0] ALU_ALUResult;
  logic [31:0] REG_SrcA;
  logic [31:0] IMEM_Inst;
  logic [31:0] DMEM_ReadData;
  logic [31:0] pc;
  
  logic CTL_RegWrite;
  logic CTL_MemWrite;
  logic [2:0] CTL_ALUControl;
  
  logic [31:0] oRdata2; // 추가
  
  alu ALU(
    .iA		(REG_SrcA),
    .iB		({{16{IMEM_Inst[15]}}, IMEM_Inst[15:0]}),
    .iF		(CTL_ALUControl),
    .oY		(ALU_ALUResult),
    .oZero	()
  );
  
  regfile REG(
    .iClk	(iClk),
    .iReset	(iReset),
    .iRaddr1(IMEM_Inst[25:21]),
    .iRaddr2(IMEM_Inst[20:16]),
    .iWaddr	(IMEM_Inst[20:16]),
    .iWe	(CTL_RegWrite),
    .iWdata	(DMEM_ReadData),
    .oRdata1(REG_SrcA),
    .oRdata2(oRdata2)
  );
  
  imem IMEM(
    .iAddr	(pc),
    .oRdata	(IMEM_Inst)
  );
  
  dmem DMEM(
    .iClk	(iClk),
    .iWe	(CTL_MemWrite),
    .iAddr	(ALU_ALUResult),
    .iWdata	(oRdata2),
    .oRdata	(DMEM_ReadData)
  );
  
  controller CTL(
    .iOp		(IMEM_Inst[31:26]),
    .oRegWrite	(CTL_RegWrite),
    .oMemWrite	(CTL_MemWrite),
    .oALUControl(CTL_ALUControl)
  );
  
  always_ff@(posedge iClk, posedge iReset)
    if(iReset)
      pc <= 0;
    else 
      pc <= pc + 4;
  
endmodule
