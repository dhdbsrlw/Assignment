module patterndet(
    input logic clk, rst, in,
    output logic out
);

    // State declaration
  typedef enum logic[8:0] {
        S0 = 3'b000, // State 0
        S1 = 3'b001, // State 1
        S2 = 3'b010, // State 2
        S3 = 3'b011, // State 3
        S4 = 3'b100  // State 4, accepting state
    } state_t;

    // Current state and next state variables
    state_t state, next_state;

    // State transition logic
    always_ff @(posedge clk or posedge rst) begin
        if (rst)
            state <= S0; // Reset to state S0
        else
            state <= next_state;
    end

    // Next state logic based on current state and input
    always_comb begin
        case (state)
            S0: next_state = in ? S1 : S0;
            S1: next_state = in ? S1 : S2;
            S2: next_state = in ? S3 : S0;
            S3: next_state = in ? S4 : S2;
            S4: next_state = in ? S1 : S2;
            default: next_state = S0;
        endcase
    end

    // Output logic: out is 1 only when in the accepting state S4
    assign out = (state == S4);

endmodule
