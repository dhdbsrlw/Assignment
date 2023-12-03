// Code your design here

`include "alu.sv"
`include "regfile.sv"
`include "imem.sv"
`include "dmem.sv"

module datapath(
  input logic iClk,
  input logic iReset,
  input logic iRegWrite,
  input logic iMemWrite,
  input logic [2:0] iALUControl
);

  logic [31:0] oRdata, iWdata, oRdata1, oY, pc, signExtend;
  logic [4:0] iWaddr;
  logic prev;
  
  // PC update
  always @(posedge iClk or iReset) begin
    if (iReset) begin
      pc = 32'h0000_0000;
    end
    else if (!iReset) begin
      if (prev == 0) begin
        pc += 4;
      end
    end
    prev = iReset ? 1'b1 : 1'b0;
  end
  
  // Instruction Memory
  imem im(
    .iAddr (pc),
    .oRdata (oRdata)
  );

  // Register File
  regfile rf(
    .iClk (iClk),
    .iReset (iReset),
    .iRaddr1 (oRdata[25:21]),
    .iRaddr2 (),
    .iWaddr (oRdata[20:16]),
    .iWe (iRegWrite),
    .iWdata (iWdata),
    .oRdata1 (oRdata1),
    .oRdata2 ()
  );
  
  // Sign Extension
  always @(posedge iClk or iReset) begin
    signExtend[15:0] = oRdata[15:0];
    if (oRdata[15])
      for (integer i = 16; i < 32; i += 1)
        signExtend[i] = oRdata[15];
    else
      for (integer i = 16; i < 32; i += 1)
        signExtend[i] = 0;
  end
  
  // ALU
  alu al(
    .iA (oRdata1),
    .iB (signExtend),
    .iF (iALUControl),
    .oY (oY),
    .oZero ()
  );
  
  // Data Memory
  dmem dm(
    .iClk (iClk),
    .iWe (iMemWrite),
    .iAddr (oY),
    .iWdata (),
    .oRdata (iWdata)
  );
  
endmodule