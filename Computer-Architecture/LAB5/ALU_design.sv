// Code your design here

module alu(
    input [31:0] iA, iB,
    input [2:0] iF,
    output logic [31:0] oY,
    output logic oZero
);

  function [31:0] perform_alu_op(input [31:0] A, B, input [2:0] F);
    begin
        case (F)
            3'b000: perform_alu_op = A & B;
            3'b001: perform_alu_op = A | B;
            3'b010: perform_alu_op = A + B;
            3'b100: perform_alu_op = A & ~B;
            3'b101: perform_alu_op = A | ~B;
            3'b110: perform_alu_op = A - B;
            3'b111: perform_alu_op = (A < B) ? 32'h0000_0001 : 32'h0000_0000;
            default: perform_alu_op = 32'h0000_0000;
        endcase
    end
endfunction

always_comb
    oY = perform_alu_op(iA, iB, iF);

assign oZero = (oY == 0);

endmodule
