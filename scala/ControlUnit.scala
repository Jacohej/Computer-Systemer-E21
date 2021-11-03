import chisel3._
import chisel3.util._

class ControlUnit extends Module {
  val io = IO(new Bundle {
    //Define the module interface here (inputs/outputs)
    val opcode = Input(UInt(32.W))
    val writeData = Output(Bool())
    val aluOP = Output(UInt(2.W))
    val ImmOrReg = Output(Bool())
    val memoryRead = Output(Bool())
    val memoryWrite = Output(Bool())
    val memoryOrAlu = Output(Bool())
    val jump = Output(Bool())
    val run = Output(Bool())
    val stop = Output(Bool())

  })
  //Implement this module here
  val aluOP = io.aluOP
  val opcode = io.opcode

  //Taking the 3 first bits from the opcode
  val opcode3 = opcode & 7.U

  switch(opcode3){
    is(0.U){aluOP := 0.U
            io.jump := false.B
            io.memoryWrite := false.B
            io.memoryRead := false.B
            io.ImmOrReg := false.B
    }
    is(1.U){aluOP := opcode3
            io.ImmOrReg := true.B
    }
    is(2.U){aluOP := opcode3}
    is(3.U){aluOP := opcode3}
    is(4.U){io.jump := true.B}
    is(5.U){io.memoryWrite := true.B}
    is(6.U){io.memoryRead := true.B}
  }




}