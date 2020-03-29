`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2020/03/30 01:14:15
// Design Name: 
// Module Name: my_fmul
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module my_fmul #(
    parameter BITWIDTH = 32
)
(
    input [BITWIDTH-1:0] ain,
    input [BITWIDTH-1:0] bin,
    input clk,
    input en,
    output [2*BITWIDTH-1:0] dout
);
    reg [2*BITWIDTH-1:0] temp;
    
    always @(posedge clk) begin
        case(en)
            1'b0: temp = 32'b0;
            1'b1: temp = temp + ain*bin;
        endcase
    end
    
    assign dout = temp;
endmodule

module test_fmul();
    parameter BITWIDTH = 32;
    
    reg [BITWIDTH-1:0] ain;
    reg [BITWIDTH-1:0] bin;
    reg clk;
    reg en;
    wire [2*BITWIDTH-1:0] dout;
    
    integer i;
    
    always #5 clk = ~clk;
    
    initial begin
        clk <= 0;
        en <= 0;
        #30;
        en <= 1;
        for(i = 0; i < 32; i = i + 1) begin
            ain = $urandom%(2**31);
            bin = $urandom%(2**31);
            #10;
        end
    end
    
    my_fmul #(BITWIDTH) MY_FMUL(
        .ain(ain),
        .bin(bin),
        .clk(clk),
        .en(en),
        .dout(dout)
    );
    
endmodule