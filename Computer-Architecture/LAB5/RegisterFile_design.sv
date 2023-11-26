module regfile(
    input iClk,    
    input iReset, 
    input [4:0] iRaddr1, iRaddr2, 
    input [4:0] iWaddr,           
    input iWe,                    
    input [31:0] iWdata,         
    output reg [31:0] oRdata1, oRdata2 
);

reg [31:0] rf[0:31]; 
integer i;        

assign oRdata1 = (iRaddr1 != 0) ? rf[iRaddr1] : 0;
assign oRdata2 = (iRaddr2 != 0) ? rf[iRaddr2] : 0;

always_ff @(posedge iClk) begin
    if (iReset) begin
        // Reset
        for (i = 0; i < 32; i = i + 1) begin
            rf[i] <= 0;
        end
    end
    else if (iWe) begin
        rf[iWaddr] <= iWdata;
    end
end

endmodule
