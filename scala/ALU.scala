import chisel3._
import chisel3.util._

class ALU extends Module {
  val io = IO(new Bundle {
    val a = Input(UInt(32.W))
    val b = Input(UInt(32.W))
    val sel = Input(UInt(2.W))
    var result = Output(UInt(32.W))
  })
  //Implement this module here
  val a = io.a
  val b = io.b
  val sel = io.sel
  var result = io.result
  //var result = Wire(UInt(16.W))
  result := 0.U

  switch(sel){
    is(0.U){result = a + b}
    is(1.U){result = a | b}
    is(2.U){result = a === b}
  }



}