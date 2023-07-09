# IMPORTS ----------------------------------------------------------------
from PyQt5.QtWidgets import *
from PyQt5 import QtCore
import numpy as np
import sys

import cie.splinekernel
# ------------------------------------------------------------------------

class ControlWidget(QWidget):

    signal_object_changed = QtCore.pyqtSignal(name='objectChanged')

    def __init__(self, parent=None):
        super(ControlWidget, self).__init__(parent)

        # Controls - mode
        labelDefineMode             = QLabel("Define points")
        self.controlDefineMode      = QRadioButton(self)
        self.controlDefineMode.toggle()
        labelEditMode               = QLabel("Edit points")
        self.controlEditMode        = QRadioButton(self)
        self.controlEditMode.toggled.connect( self.updateSettings )

        # Controls - polynomial order
        labelPolynomialOrder        = QLabel("Polynomial order")
        self.controlPolynomialOrder = QSpinBox(self)
        self.controlPolynomialOrder.setMinimum(1)
        self.controlPolynomialOrder.setMaximum(10)
        self.controlPolynomialOrder.setValue(3)
        self.controlPolynomialOrder.valueChanged.connect(self.updateSettings)

        # Controls - snap to grid
        labelSnapToGrid            = QLabel("Snap to grid")
        self.controlSnapToGrid      = QCheckBox(self)
        self.controlSnapToGrid.toggled.connect( self.updateSettings )

        # Controls layout
        gbox = QGridLayout()

        gbox.addWidget(labelPolynomialOrder, 0, 0)
        gbox.addWidget(self.controlPolynomialOrder, 0, 1)

        gbox.addWidget(labelDefineMode,1,0)
        gbox.addWidget(self.controlDefineMode,1,1)
        gbox.addWidget(labelEditMode,2,0)
        gbox.addWidget(self.controlEditMode,2,1)

        gbox.addWidget(labelSnapToGrid)
        gbox.addWidget(self.controlSnapToGrid)

        vbox = QVBoxLayout()
        vbox.addLayout(gbox)
        vbox.addStretch(1.0)
        self.setLayout(vbox)

        # Control values
        self.settings = {
            'mode'              : 'define',
            'polynomialOrder'   : 3,
            'snapToGrid'        : False
        } 
        

    def updateSettings(self):
        # Read polynomial order
        self.settings['polynomialOrder'] = self.controlPolynomialOrder.value()
        # Read radio button group (mode)
        if self.controlDefineMode.isChecked():
            self.settings['mode'] = 'define'
        elif self.controlEditMode.isChecked():
            self.settings['mode'] = 'edit'
        # Read snap to grid
        self.settings['snapToGrid'] = self.controlSnapToGrid.isChecked()
        # Emit object changed signal
        self.signal_object_changed.emit()


class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.resize(1024, 768)
        self.setWindowTitle('Spline Builder')

        splitter = QSplitter(QtCore.Qt.Horizontal)

        self.canvas = pysplinekernel.CanvasSpline2D()
        self.canvas.create_native()
        self.canvas.native.setParent(self)

        self.controlWidget = ControlWidget()
        splitter.addWidget(self.controlWidget)
        splitter.addWidget(self.canvas.native)

        self.setCentralWidget(splitter)

        self.controlWidget.signal_object_changed.connect(self.updateView)
        self.updateView()


    def updateView(self):
        self.canvas.updateSettings(**self.controlWidget.settings)