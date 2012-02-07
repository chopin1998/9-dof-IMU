#!/usr/bin/python


import sys
import math

from PyQt4 import QtCore, QtGui, QtOpenGL
from OpenGL import GL
import random
import time
import serial



class Window(QtGui.QWidget):
    
    RAD_REG_OP = 180.0 / 3.14159265359
    DEVICE = '/dev/ttyUSB0'
    BAUDRATE = 576000
    
    def __init__(self):
        super(Window, self).__init__()

        self.glWidget = GLWidget()
        self.glWidget.setXRotation(0)
        self.glWidget.setXRotation(0)
        self.glWidget.setZRotation(0)
        
        self.init_dev()

        mainLayout = QtGui.QHBoxLayout()
        mainLayout.addWidget(self.glWidget)
        self.setLayout(mainLayout)

        self.setWindowTitle("imu collector")
        
        self.startTimer(2)
        

    def init_dev(self):
        try:
            self.device = serial.Serial(self.DEVICE, self.BAUDRATE, timeout=0.2)
        except Exception, ex:
            print 'open serial port error:\n %s' %(ex)
            sys.exit(-1)
        
        self.device.write('[accel,dump][gyro,dump]')
        self.device.flush()
    
    def timerEvent(self, ev):
        raw = self.device.readline().strip().split(':')
        if len(raw) == 2 and raw[0] == 'tilt':
            raw = raw[1].split(',')
            try:
               data = map(float, raw)
            except:
                print 'unknow data', raw
            self.process_data(data)
        else:
            print 'unknow data', raw
    
    def process_data(self, data):
        
        x, y, z = data
        x *= self.RAD_REG_OP
        y *= self.RAD_REG_OP
        
        print time.time(), '\t', x, y
        self.glWidget.setXRotation(x)
        self.glWidget.setYRotation(y)
    
    def closeEvent(self, ev):
        self.device.write('[gyro,dump,0][accel,dump,0]')
        

class GLWidget(QtOpenGL.QGLWidget):

    def __init__(self, parent=None):
        super(GLWidget, self).__init__(parent)

        self.obj = 0
        self.xRot = 0
        self.yRot = 0
        self.zRot = 0
        
        self.lastPos = QtCore.QPoint()

        self.trolltechGreen = QtGui.QColor.fromCmykF(0.40, 0.0, 1.0, 0.0)
        self.trolltechPurple = QtGui.QColor.fromCmykF(0.39, 0.39, 0.0, 0.0)
    
    def minimumSizeHint(self):
        return QtCore.QSize(320, 210)

    def sizeHint(self):
        return QtCore.QSize(640, 420)

    def setXRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.xRot:
            self.xRot = angle
            self.updateGL()

    def setYRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.yRot:
            self.yRot = angle
            self.updateGL()

    def setZRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.zRot:
            self.zRot = angle
            self.updateGL()

    def initializeGL(self):
        self.qglClearColor(self.trolltechPurple.dark())
        self.obj = self.makeObject()
        GL.glShadeModel(GL.GL_FLAT)
        GL.glEnable(GL.GL_DEPTH_TEST)
        GL.glEnable(GL.GL_CULL_FACE)

    def paintGL(self):
        GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)
        GL.glLoadIdentity()
        GL.glTranslated(0.0, 0.0, -10.0)
        GL.glRotated(self.xRot / 1.0, 1.0, 0.0, 0.0)
        GL.glRotated(self.yRot / 1.0, 0.0, 1.0, 0.0)
        GL.glRotated(self.zRot / 1.0, 0.0, 0.0, 1.0)
        GL.glCallList(self.obj)

    def resizeGL(self, width, height):
        side = min(width, height)
        if side < 0:
            return

        GL.glViewport((width - side) / 2, (height - side) / 2, side, side)

        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        GL.glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0)
        GL.glMatrixMode(GL.GL_MODELVIEW)

    def makeObject(self):
        genList = GL.glGenLists(1)
        GL.glNewList(genList, GL.GL_COMPILE)

        GL.glBegin(GL.GL_QUADS)

        """
        x1 = +0.06
        y1 = -0.14
        x2 = +0.14
        y2 = -0.06
        x3 = +0.08
        y3 = +0.00
        x4 = +0.30
        y4 = +0.22
        """
        x1 = +0.0
        y1 = -0.0
        x2 = +0.0
        y2 = -0.0
        x3 = +0.0
        y3 = +0.0
        x4 = +0.2
        y4 = +0.0

        self.quad(x1, y1, x2, y2, y2, x2, y1, x1)
        self.quad(x3, y3, x4, y4, y4, x4, y3, x3)

        self.extrude(x1, y1, x2, y2)
        self.extrude(x2, y2, y2, x2)
        self.extrude(y2, x2, y1, x1)
        self.extrude(y1, x1, x1, y1)
        self.extrude(x3, y3, x4, y4)
        self.extrude(x4, y4, y4, x4)
        self.extrude(y4, x4, y3, x3)

        NumSectors = 200

        for i in range(NumSectors):
            angle1 = (i * 2 * math.pi) / NumSectors
            x5 = 0.30 * math.sin(angle1)
            y5 = 0.30 * math.cos(angle1)
            x6 = 0.20 * math.sin(angle1)
            y6 = 0.20 * math.cos(angle1)

            angle2 = ((i + 1) * 2 * math.pi) / NumSectors
            x7 = 0.20 * math.sin(angle2)
            y7 = 0.20 * math.cos(angle2)
            x8 = 0.30 * math.sin(angle2)
            y8 = 0.30 * math.cos(angle2)

            self.quad(x5, y5, x6, y6, x7, y7, x8, y8)

            self.extrude(x6, y6, x7, y7)
            self.extrude(x8, y8, x5, y5)

        GL.glEnd()
        GL.glEndList()

        return genList

    def quad(self, x1, y1, x2, y2, x3, y3, x4, y4):
        self.qglColor(self.trolltechGreen)

        GL.glVertex3d(x1, y1, -0.05)
        GL.glVertex3d(x2, y2, -0.05)
        GL.glVertex3d(x3, y3, -0.05)
        GL.glVertex3d(x4, y4, -0.05)

        GL.glVertex3d(x4, y4, +0.05)
        GL.glVertex3d(x3, y3, +0.05)
        GL.glVertex3d(x2, y2, +0.05)
        GL.glVertex3d(x1, y1, +0.05)

    def extrude(self, x1, y1, x2, y2):
        self.qglColor(self.trolltechGreen.dark(250 + int(100 * x1)))

        GL.glVertex3d(x1, y1, +0.05)
        GL.glVertex3d(x2, y2, +0.05)
        GL.glVertex3d(x2, y2, -0.05)
        GL.glVertex3d(x1, y1, -0.05)

    def normalizeAngle(self, angle):
        while angle < 0:
            angle += 360 * 1
        while angle > 360 * 1:
            angle -= 360 * 1
        return angle


if __name__ == '__main__':

    app = QtGui.QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
