module controller(
  input logic [5:0] iOp,
  output logic oRegWrite,
  output logic oMemWrite,
  output logic [2:0] oALUControl
);

  // 초기값
  initial begin
    oALUControl = 3'b010;
  end
  
  // 업데이트
  always_comb begin
    oRegWrite = 0;
    oMemWrite = 0;

    if (iOp == 6'b100011) begin
      oRegWrite = 1;
    end
  end
  
endmodule