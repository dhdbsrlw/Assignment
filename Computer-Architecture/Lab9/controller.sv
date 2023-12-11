module controller(
  input logic [5:0] iOp,
  input logic [5:0] func, 
  output logic oRegWrite,      
  output logic oMemWrite,       
  output logic RegDst,
  output logic ALUSrc,           
  output logic MemtoReg,   
  output logic [2:0] oALUControl     
);

  logic [1:0] aluop; 

  always_comb begin
    oRegWrite = 0;
    oMemWrite = 0;
    aluop = 2'b00;
    RegDst = 0;
    ALUSrc = 0;
    MemtoReg = 0;

    case(iOp)
      6'b100011: begin 
        oRegWrite = 1;
        ALUSrc = 1;
        MemtoReg = 1;
      end
      6'b101011: begin 
        oMemWrite = 1;
        ALUSrc = 1;
      end
      6'b000000: begin // R-type Instruction
        oRegWrite = 1;
        aluop = 2'b10;
        RegDst = 1;
      end
    endcase
  end
  
  always_comb begin
    oALUControl = 3'b000; 

    case(aluop)
      2'b00: oALUControl = 3'b010; 

      2'b10: // for R-type instructions
        case(func)
          6'b100000: oALUControl = 3'b010; 
          6'b100010: oALUControl = 3'b110; 
          6'b100100: oALUControl = 3'b000; 
          6'b100101: oALUControl = 3'b001; 
          6'b101010: oALUControl = 3'b111; 
        endcase
    endcase
  end

endmodule
