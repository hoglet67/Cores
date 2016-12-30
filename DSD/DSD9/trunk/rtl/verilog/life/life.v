// ============================================================================
//        __
//   \\__/ o\    (C) 2016  Robert Finch, Waterloo
//    \  __ /    All rights reserved.
//     \/_//     robfinch<remove>@finitron.ca
//       ||
//
//	life.v
//  Conway's Game of Life
//		
//
// This source file is free software: you can redistribute it and/or modify 
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or     
// (at your option) any later version.                                      
//                                                                          
// This source file is distributed in the hope that it will be useful,      
// but WITHOUT ANY WARRANTY; without even the implied warranty of           
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
// GNU General Public License for more details.                             
//                                                                          
// You should have received a copy of the GNU General Public License        
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    
//                                                                          
//
// ============================================================================
//

// Memory for life cells.

module liferam(wclk, wce, wadr, i, rclk, rce, ra0, ra1, ra2, rclk3, ra3, o0, o1, o2, o3);
input wclk;
input wce;
input [7:0] wadr;
input [511:0] i;
input rclk;
input rce;
input [7:0] ra0;
input [7:0] ra1;
input [7:0] ra2;
input rclk3;
input [7:0] ra3;
output [511:0] o0;
output [511:0] o1;
output [511:0] o2;
output [511:0] o3;

reg [511:0] mem [0:255];
reg [7:0] rra0, rra1, rra2, rra3;

always @(posedge wclk)
    if (wce) mem[wadr] <= i;

always @(posedge rclk)
    rra0 <= ra0;
always @(posedge rclk)
    rra1 <= ra1;
always @(posedge rclk)
    rra2 <= ra2;
always @(posedge rclk3)
    rra3 <= ra3;

assign o0 = mem[rra0];
assign o1 = mem[rra1];
assign o2 = mem[rra2];
assign o3 = mem[rra3];

endmodule

// Calculate whether a cell is alive or dead.
module lifecalc(self, n, alive);
input self;
input [7:0] n;
output alive;

wire [3:0] sum = n[0] + n[1] + n[2] + n[3] + n[4] + n[5] + n[6] + n[7];
assign alive = self ? (sum==4'd2 || sum===4'd3) : (sum==4'd3);

endmodule

// Calculate life for an entire row of cells.

module lifecalc_parallel(row0, row1, row2, alive);
input [511:0] row0;
input [511:0] row1;
input [511:0] row2;
output [511:0] alive;

genvar g;
generate
begin : lifecal
for (g = 0; g < 512; g = g + 1)
begin : lf1
lifecalc ul (
    .self(row1[g]),
    .n({
    (g==511 ? {row0[0],row0[511],row0[510]} :
       g==0 ? {row0[1],row0[0],row0[511]} :  
               row0[(g+1):(g-1)]),
    (g==511 ? {row1[0],row1[510]} :
       g==0 ? {row1[1],row1[511]} :  
              {row1[(g+1)],row1[(g-1)]}),
    (g==511 ? {row2[0],row2[511],row2[510]} :
       g==0 ? {row2[1],row2[0],row2[511]} :  
               row2[(g+1):(g-1)])}),
    .alive(alive[g])
);
end
end
endgenerate
endmodule

module lifegame(rst_i,clk_i,cyc_i,stb_i,ack_o,we_i,adr_i,dat_i,dat_o,vclk,vsync,hsync,rgb_i,rgb_o);
input rst_i;
input clk_i;
input cyc_i;
input stb_i;
output ack_o;
input we_i;
input [31:0] adr_i;
input [31:0] dat_i;
output [31:0] dat_o;
input vclk;
input vsync;
input hsync;
input [23:0] rgb_i;
output reg [23:0] rgb_o;

reg [7:0] radr;
reg [7:0] wadr0,wadr1,wadr2,wadr3;
wire [511:0] alive;
reg [511:0] alive1,alive2,alive3;
reg [9:0] scanline;
reg [11:0] dotcnt;
wire pe_hsync,pe_vsync;
wire [511:0] row0, row1, row2, row3;
reg [511:0] d;
reg [511:0] q,q1;
reg load,calc,disp;
reg [7:0] load_row;

wire cs = cyc_i && stb_i && (adr_i[31:16]==16'hFFD3);
assign ack_o = cs;

assign dat_o = 32'h0000;

genvar g;
generate
begin
for (g = 0; g < 16; g = g + 1)
begin
always @(posedge clk_i)
if (rst_i) begin
    calc <= 1'b0;
    load <= 1'b0;
    disp <= 1'b1;
end
else begin
    load <= 1'b0;
    if (cs) begin
        if (we_i)
            casex(adr_i[6:2])
            5'b0xxxx:   d[(g+1)*32-1:g*32] <= dat_i;
            5'b10000:   load_row <= dat_i[7:0];
            5'b10001:   begin
                        load <= dat_i[0];
                        calc <= dat_i[1];
                        disp <= dat_i[2];
                        end
            endcase
    end
end
end
end
endgenerate

liferam u1
(
    .wclk(clk_i),
    .wce(load|calc),
    .wadr(load ? load_row : wadr3),
    .i(load ? d : alive3),
    .rclk(clk_i),
    .rce(1'b1),
    .ra0(radr-8'd1),
    .ra1(radr),
    .ra2(radr+8'd1),
    .rclk3(pe_hsync),
    .ra3(scanline[8:1]),
    .o0(row0),
    .o1(row1),
    .o2(row2),
    .o3(row3)
);

lifecalc_parallel u2 (row0, row1, row2, alive);

always @(posedge clk_i)
if (rst_i)
    radr <= 8'd0;
else
    radr <= radr + 8'd1;

// Pipeline the write-back. The write-back needs to occur at least three cycles
// after the read and calculation. So that the calculation for a given row isn't
// affected by the update of the previous row. Need to simulate an everything
// is calculated in parallel environment.

always @(posedge clk_i)
begin
    alive1 <= alive;
    alive2 <= alive1;
    alive3 <= alive2;
    wadr0 <= radr;
    wadr1 <= wadr0;
    wadr2 <= wadr1;
    wadr3 <= wadr2;
end

//-------------------------------------------------------------
// Video Stuff
//-------------------------------------------------------------

edge_det edh1
(
	.rst(rst_i),
	.clk(vclk),
	.ce(1'b1),
	.i(hsync),
	.pe(pe_hsync),
	.ne(),
	.ee()
);

edge_det edv1
(
	.rst(rst_i),
	.clk(vclk),
	.ce(1'b1),
	.i(vsync),
	.pe(pe_vsync),
	.ne(),
	.ee()
);

wire blank = scanline[9] || (dotcnt < 140) || (dotcnt > 1165);

always @(posedge vclk)
    if (pe_vsync)
        scanline <= 10'h380;
    else if (pe_hsync)
        scanline <= scanline + 10'd1;
always @(posedge vclk)
    if (pe_hsync)
        dotcnt <= 12'd0;
    else
        dotcnt <= dotcnt + 12'd1;

always @(posedge vclk)
    if (pe_hsync)
        q <= row3;
    else if (!blank & dotcnt[0])
        q <= {q[510:0],1'b0};

always @(posedge vclk)
    if (blank)
        rgb_o <= rgb_i;
    else
        rgb_o <= q[511] ? 24'hFF_FF_FF : 24'h00_00_00; 

endmodule
