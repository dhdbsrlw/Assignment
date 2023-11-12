// Code your testbench here
// or browse Examples

module testbench();
  logic clk, rst, in;
  logic out;
  // instantiate device under test
  patterndet dut(clk, rst, in, out);

  always     // no sensitivity list, so it always executes
    begin
      clk = 1; #5; clk = 0; #5;
    end

  initial begin
    $dumpfile("dump.vcd"); $dumpvars;
    in = 0; rst = 1; #21; rst = 0; #10
    in = 1; #30
    in = 0; #40
    in = 1; #10
    in = 0; #10
    in = 1; #40
    in = 0; 
  end
endmodule 
