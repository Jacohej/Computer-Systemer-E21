import chisel3._
import chisel3.util._

class ProgramCounter extends Module {
  val io = IO(new Bundle {
    val stop = Input(Bool())
    val jump = Input(Bool())
    val run = Input(Bool())
    val programCounterJump = Input(UInt(16.W))
    var programCounter = Output(UInt(16.W))
  })

  //Implement this module here (respect the provided interface, since it used by the tester)
  val andgate = io.stop | ~io.run
  val regcount = Reg {
    UInt(16.W)
  }
  val counterup = 1.U + regcount
  io.programCounter := regcount

  when (andgate){
    regcount := regcount
  } .elsewhen (io.jump){
    regcount := counterup
  } .otherwise {
    regcount := io.programCounterJump
  }


}