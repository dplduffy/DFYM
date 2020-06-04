# **DFYM**
Duffy Modular for VCV Rack

## **Quantum Launch**
Qauntum Launch is a buffered mute that was inspired by the clip launching functionality of Ableton Live.  Signal flows from In and Clk at the top of the module, through the Trig / Mute state, to the Out and Fdbk ports at the bottom.
- #### Inputs
  - In
    - Any signal you want to pass to the Out port.
  - Clk In
    - The clock that will be used as quantization to toggle mute state.
  - Trig In
    - Trigger to toggle mute state.
- #### Trig
  - The Trig button toggles the state of the channel and the Trig LED displays the state of the channel. Channel state ON means signals are being passed from In to Out. ON state is indicated by Trig LED being Green. Channel OFF state means the channel is muted and no signals are being passed, indicated by an off LED.
  - State is quantized to 4 beats of the Clk In. When Trig is activated, either via Trig In or the Trig button, the Trig LED will pulse every beat signaling the state is about to change.
- #### Outputs
  - Fdbk
    - Feedback that state is ON or OFF.  When state is ON Fdbk will send 10v. When OFF it will be 0v. This was included as a way to set lights on midi controllers.
  - Out
    - If a signal is connected to the In port it will be passed to Out when state is ON. If no signal is connected to the In port Clk will be passed to Out.