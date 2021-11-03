import chisel3._
import chisel3.util._

class RegisterFile extends Module {
  val io = IO(new Bundle {
    //Define the module interface here (inputs/outputs)
    val aSel = Input(UInt(4.W))
    val bSel = Input(UInt(4.W))
    val writeData = Input(UInt(32.W))
    val writeSel = Input(UInt(4.W))
    val writeEnable = Input(Bool())

    var a = Output(UInt(32.W))
    var b = Output(UInt(32.W))
  })
  //Implement this module here



  var reg0 = RegInit(UInt(32.W))
  var reg1 = RegInit(0.U(32.W))
  var reg2 = RegInit(0.U(32.W))
  var reg3 = RegInit(0.U(32.W))
  var reg4 = RegInit(0.U(32.W))
  var reg5 = RegInit(0.U(32.W))
  var reg6 = RegInit(0.U(32.W))
  var reg7 = RegInit(0.U(32.W))
  var reg8 = RegInit(0.U(32.W))
  var reg9 = RegInit(0.U(32.W))
  var reg10 = RegInit(0.U(32.W))
  var reg11 = RegInit(0.U(32.W))
  var reg12 = RegInit(0.U(32.W))
  var reg13 = RegInit(0.U(32.W))
  var reg14 = RegInit(0.U(32.W))
  var reg15 = RegInit(0.U(32.W))

  //Selecting register for a
  switch(io.aSel){
    is(0.U){io.a = reg0}
    is(1.U){io.a = reg1}
    is(2.U){io.a = reg2}
    is(3.U){io.a = reg3}
    is(4.U){io.a = reg4}
    is(5.U){io.a = reg5}
    is(6.U){io.a = reg6}
    is(7.U){io.a = reg7}
    is(8.U){io.a = reg8}
    is(9.U){io.a = reg9}
    is(10.U){io.a = reg10}
    is(11.U){io.a = reg11}
    is(12.U){io.a = reg12}
    is(13.U){io.a = reg13}
    is(14.U){io.a = reg14}
    is(15.U){io.a = reg15}
  }

//Selecting register for b
  switch(io.bSel){
    is(0.U){io.b = reg0}
    is(1.U){io.b = reg1}
    is(2.U){io.b = reg2}
    is(3.U){io.b = reg3}
    is(4.U){io.b = reg4}
    is(5.U){io.b = reg5}
    is(6.U){io.b = reg6}
    is(7.U){io.b = reg7}
    is(8.U){io.b = reg8}
    is(9.U){io.b = reg9}
    is(10.U){io.b = reg10}
    is(11.U){io.b = reg11}
    is(12.U){io.b = reg12}
    is(13.U){io.b = reg13}
    is(14.U){io.b = reg14}
    is(15.U){io.b = reg15}
  }

when(io.writeEnable){
  switch(io.writeSel){
    is(0.U){reg0 = io.writeData}
    is(1.U){reg1 = io.writeData}
    is(2.U){reg2 = io.writeData}
    is(3.U){reg3 = io.writeData}
    is(4.U){reg4 = io.writeData}
    is(5.U){reg5 = io.writeData}
    is(6.U){reg6 = io.writeData}
    is(7.U){reg7 = io.writeData}
    is(8.U){reg8 = io.writeData}
    is(9.U){reg9 = io.writeData}
    is(10.U){reg10 = io.writeData}
    is(11.U){reg11 = io.writeData}
    is(12.U){reg12 = io.writeData}
    is(13.U){reg13 = io.writeData}
    is(14.U){reg14 = io.writeData}
    is(15.U){reg15 = io.writeData}
  }

}
















}