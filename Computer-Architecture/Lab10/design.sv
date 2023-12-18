// Code your design here
`include "alu.sv"
`include "regfile.sv"
`include "imem.sv"
`include "dmem.sv"
`include "controller.sv"

// D (Decode)
// E (Execute)
// M (Memory)
// W (Write-back) 

module mips(
  input logic iClk,
  input logic iReset
);

  logic [31:0] ALU_ALUResultE;
  logic [31:0] ALU_ALUResultM;
  logic [31:0] ALU_ALUResultW;
  logic [31:0] REG_SrcAD;
  logic [31:0] REG_SrcAE;
  logic [31:0] REG_WriteData; 
  logic [31:0] IMEM_InstF;
  logic [31:0] IMEM_InstD;
  logic [31:0] DMEM_ReadDataM;
  logic [31:0] DMEM_ReadDataW;
  logic [31:0] pc;
  // logic [4:0] WriteReg; // 보류
  logic [4:0] WriteRegE; 
  logic [4:0] WriteRegM; 
  logic [4:0] WriteRegW; 
  logic [31:0] SrcBE;
  logic [31:0] Result;
  logic [31:0] SignImmD;
  logic [31:0] SignImmE;
  logic [4:0] Rt;
  logic [4:0] RtE;  
  logic [4:0] Rd;  
  logic [4:0] RdE;
  
  logic CTL_RegWriteD;
  logic CTL_RegWriteE;
  logic CTL_RegWriteM;
  logic CTL_RegWriteW;
  logic CTL_MemWriteD;
  logic CTL_MemWriteE;
  logic CTL_MemWriteM;
  logic CTL_RegDstD;
  logic CTL_RegDstE;
  logic CTL_ALUSrcD;
  logic CTL_ALUSrcE;
  logic CTL_MemtoRegD;
  logic CTL_MemtoRegE;
  logic CTL_MemtoRegM;
  logic CTL_MemtoRegW; 
  logic [2:0] CTL_ALUControlD;
  logic [2:0] CTL_ALUControlE;
  
  assign Rt = IMEM_InstD[20:16];
  assign Rd = IMEM_InstD[15:11];
  assign WriteRegE = CTL_RegDstE ? RdE : RtE;
  assign SignImmD = {{16{IMEM_InstD[15]}}, IMEM_InstD[15:0]};
  assign SrcBE = CTL_ALUSrcE ? SignImmE : REG_WriteData;
  assign Result = CTL_MemtoRegW ? DMEM_ReadDataW : ALU_ALUResultW; // 수정
  
  alu ALU(
    .iA		(REG_SrcAE),
    .iB		(SrcBE),
    .iF		(CTL_ALUControlE),
    .oY		(ALU_ALUResultE),
    .oZero	()
  );
  
  regfile REG(
    .iClk	(iClk),
    .iReset	(iReset),
    .iRaddr1(IMEM_InstD[25:21]),
    .iRaddr2(IMEM_InstD[20:16]),
    .iWaddr	(WriteRegE),
    .iWe	(CTL_RegWriteE), // 수정
    .iWdata	(Result),
    .oRdata1(REG_SrcAD),
    .oRdata2(REG_WriteData)
  );
  
  imem IMEM( // 수정 없음
    .iAddr	(pc),
    .oRdata	(IMEM_InstF)
  );
  
  dmem DMEM(
    .iClk	(iClk),
    .iReset	(iReset),
    .iWe	(CTL_MemWriteM), // 수정
    .iAddr	(ALU_ALUResultM),
    .iWdata	(REG_WriteData),
    .oRdata	(DMEM_ReadDataM)
  );
  
  controller CTL(
    .iOp		(IMEM_InstD[31:26]),
    .iFunc		(IMEM_InstD[5:0]),
    .oRegWrite	(CTL_RegWriteD), 
    .oMemWrite	(CTL_MemWriteD), 
    .oRegDst	(CTL_RegDstD),
    .oALUSrc	(CTL_ALUSrcD),
    .oMemtoReg	(CTL_MemtoRegD),
    .oALUControl(CTL_ALUControlD)
  );
  
  always_ff@(posedge iClk, posedge iReset) // 수정 없음
    if(iReset)
      pc <= 0;
    else 
      pc <= pc + 4;

    always_ff@(posedge iClk, posedge iReset)
      if(iReset) begin
      	  IMEM_InstD <= 0;
          REG_SrcAE <= 0;
          SignImmE <= 0;
          CTL_ALUSrcE <= 0;
        
          // ALU_ALUResultE <= 0;
          ALU_ALUResultM <= 0; 
          ALU_ALUResultW <= 0; 
          CTL_ALUControlE <= 0;
          CTL_MemtoRegE <= 0;
          CTL_MemtoRegM <= 0;
          CTL_MemtoRegW <= 0;
          CTL_MemWriteE <= 0;
          CTL_MemWriteM <= 0;
          CTL_RegWriteE <= 0;
          CTL_RegWriteM <= 0;
          CTL_RegWriteW <= 0;
          CTL_RegDstE <= 0;
          RtE <= 0;
          RdE <= 0;
          WriteRegM <= 0;
          WriteRegW <= 0;
          DMEM_ReadDataW <= 0;
        end 
  		
  		else begin 
          IMEM_InstD <= IMEM_InstF;
          REG_SrcAE <= REG_SrcAD;
          SignImmE <= SignImmD;
          CTL_ALUSrcE <= CTL_ALUSrcD;
          ALU_ALUResultM <= ALU_ALUResultE;
          ALU_ALUResultW <= ALU_ALUResultM;
          CTL_ALUControlE <= CTL_ALUControlD;
          CTL_MemtoRegE <= CTL_MemtoRegD;
          CTL_MemtoRegM <= CTL_MemtoRegE;
          CTL_MemtoRegW <= CTL_MemtoRegM;
          CTL_MemWriteE <= CTL_MemWriteD;
          CTL_MemWriteM <= CTL_MemWriteE;
          CTL_RegWriteE <= CTL_RegWriteD;
          CTL_RegWriteM <= CTL_RegWriteE;
          CTL_RegWriteW <= CTL_RegWriteM;
          CTL_RegDstE <= CTL_RegDstD;
          RtE <= Rt;
          RdE <= Rd;
          WriteRegM <= WriteRegE;
          WriteRegW <= WriteRegM;
          DMEM_ReadDataW <= DMEM_ReadDataM;
        end
  
endmodule
