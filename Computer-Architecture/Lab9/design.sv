// Code your design here

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
  logic [31:0] REG_WriteData;
  logic [31:0] IMEM_Inst;
  logic [31:0] DMEM_ReadData;
  logic [31:0] pc;
  
  logic CTL_RegWrite;
  logic CTL_MemWrite;
  logic [2:0] CTL_ALUControl;
  
  alu ALU(
    .iA		(REG_SrcA),
    .iB		(ALUSrc ? {{16{IMEM_Inst[15]}}, IMEM_Inst[15:0]} : REG_WriteData),
    .iF		(CTL_ALUControl),
    .oY		(ALU_ALUResult),
    .oZero	()
  );
  
  regfile REG(
    .iClk	(iClk),
    .iReset	(iReset),
    .iRaddr1(IMEM_Inst[25:21]),
    .iRaddr2(IMEM_Inst[20:16]),
    .iWaddr	(RegDst ? IMEM_Inst[15:11] : IMEM_Inst[20:16]),
    .iWe	(CTL_RegWrite),
    .iWdata	(MemtoReg ? DMEM_ReadData : ALU_ALUResult),
    .oRdata1(REG_SrcA),
    .oRdata2(REG_WriteData)
  );
  
  imem IMEM(
    .iAddr	(pc),
    .oRdata	(IMEM_Inst)
  );
  
  dmem DMEM(
    .iClk	(iClk),
    .iWe	(CTL_MemWrite),
    .iAddr	(ALU_ALUResult),
    .iWdata	(REG_WriteData),
    .oRdata	(DMEM_ReadData)
  );
  
  controller CTL(
    .iOp		(IMEM_Inst[31:26]),
    .func		(IMEM_Inst[5:0]),
    .oRegWrite	(CTL_RegWrite),
    .oMemWrite	(CTL_MemWrite),
    .oALUControl(CTL_ALUControl),
    .RegDst 	(RegDst),
    .ALUSrc 	(ALUSrc),
    .MemtoReg 	(MemtoReg)
  );
  
  always_ff@(posedge iClk, posedge iReset) begin
    if(iReset)
      pc <= 0;
    else 
      pc <= pc + 4;
  end
  
endmodule
