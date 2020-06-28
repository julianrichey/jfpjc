//`include "hm01b0_sim.v"
//`include "ice40_ebr.v"

`timescale 1ns/100ps

module hm01b0_ingester_tb();
    reg nreset;

    // hm01b0
    reg hm01b0_mclk;          // 100kHz
    //reg hm01b0_nreset;
    wire       hm01b0_pixclk;
    wire [7:0] hm01b0_pixdata;
    wire       hm01b0_hsync;
    wire       hm01b0_vsync;
    hm01b0_sim hm01b0(.mclk(hm01b0_mclk),
                      .nreset(nreset),

                      .clock(hm01b0_pixclk),
                      .pixdata(hm01b0_pixdata),
                      .hsync(hm01b0_hsync),
                      .vsync(hm01b0_vsync));

    // ingester
    reg clock;                // 1MHz

    wire [2:0] output_ebr_select;
    wire       frontbuffer_select;
    wire [8:0] output_write_addr;
    wire [7:0] output_pixval;
    wire       wren;

    hm01b0_ingester hm01b0_ing(.clock(clock),
                               .nreset(nreset),

                               .hm01b0_pixclk(hm01b0_pixclk),
                               .hm01b0_pixdata(hm01b0_pixdata),
                               .hm01b0_hsync(hm01b0_hsync),
                               .hm01b0_vsync(hm01b0_vsync),

                               .output_block_select(output_ebr_select),
                               .frontbuffer_select(frontbuffer_select),
                               .output_write_addr(output_write_addr),
                               .output_pixval(output_pixval),
                               .wren(wren));

    genvar gi;
    integer gij;
    wire block_wren[0:4];
    generate
        for (gi = 0; gi < 5; gi = gi + 1) begin: ebrs
            assign block_wren[gi] = ((output_ebr_select == gi) && wren);
            ice40_ebr jpeg_buffer(.din(output_pixval),
                                  .write_en(block_wren[gi]),
                                  .waddr(output_write_addr),
                                  .wclk(clock),
                                  .raddr(9'h0),
                                  .rclk(1'h0),
                                  .dout());

            // zero buffer
            initial begin
                for (gij = 0; gij < 512; gij = gij + 1) begin
                    jpeg_buffer.mem[gij] = 'h0;
                end
            end
        end
    endgenerate

    // generate hm01b0 clock
    always begin
        #1250; hm01b0_mclk = ~hm01b0_mclk; #1250;
    end

    // generate system clock
    always begin
        #250; clock = ~clock; #250;
    end

    integer i;
    integer j;
    initial begin
        $dumpfile("hm01b0_ingester_tb.vcd");
        $dumpvars(0, hm01b0_ingester_tb);

        // initialize memories
        $readmemh("./pictures/checkerboard_highfreq.hex", hm01b0.hm01b0_image);

        //
        clock = 1'b0;
        hm01b0_mclk = 1'b0;
        nreset = 1'b0;
        #5000;
        nreset = 1'b1;

        //while (!((hm01b0_vsync == 0) && (hm01b0_hsync == 0))) begin
        while (!((hm01b0_hsync == 0))) begin
            #1000;
        end
        #50000;

        $writememh("hm01b0_ingester_0.hex", ebrs[0].jpeg_buffer.mem);
        $writememh("hm01b0_ingester_1.hex", ebrs[1].jpeg_buffer.mem);
        $finish;
    end
endmodule