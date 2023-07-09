# IMPORTS ----------------------------------------------------------------
from PyQt5.QtWidgets import *
from PyQt5 import QtCore

import cie.splinekernel as splinekernel
# ------------------------------------------------------------------------

class ControlWidget(QWidget):

    signal_object_changed = QtCore.pyqtSignal(name='objectChanged')

    def __init__(self, parent=None):
        super(ControlWidget, self).__init__(parent)

        self.labels     = {}
        self.controls   = {}

        # Controls - modes
        self.labels['defineMode']   = QLabel("Define points")
        self.controls['defineMode'] = QRadioButton(self)
        self.controls['defineMode'].toggle()
        self.labels['editMode']     = QLabel("Edit points")
        self.controls['editMode']   = QRadioButton(self)
        self.controls['editMode'].toggled.connect( self.updateSettings )

        # Controls - polynomial order
        self.labels['polynomialOrder']      = QLabel("Polynomial order")
        self.controls['polynomialOrder']    = QSpinBox(self)
        self.controls['polynomialOrder'].setMinimum(1)
        self.controls['polynomialOrder'].setMaximum(10)
        self.controls['polynomialOrder'].setValue(3)
        self.controls['polynomialOrder'].valueChanged.connect(self.updateSettings)

        # Controls - snap to grid
        self.labels['snapToGrid']   = QLabel("Snap to grid")
        self.controls['snapToGrid'] = QCheckBox(self)
        self.controls['snapToGrid'].toggled.connect( self.updateSettings )

        # Controls layout
        gbox = QGridLayout()

        for index,item in enumerate(self.controls.items()):
            gbox.addWidget( self.labels[item[0]],   index, 0 )
            gbox.addWidget( item[1],                index, 1 )

        vbox = QVBoxLayout()
        vbox.addLayout(gbox)
        vbox.addStretch(1)
        self.setLayout(vbox)

        # Control values
        self.settings = {
            'mode'              : 'define',
            'polynomialOrder'   : 3,
            'snapToGrid'        : False
        }


    def updateSettings(self):
        # Read polynomial order
        self.settings['polynomialOrder'] = self.controls['polynomialOrder'].value()
        # Read radio button group (mode)
        if self.controls['defineMode'].isChecked():
            self.settings['mode'] = 'define'
        elif self.controls['editMode'].isChecked():
            self.settings['mode'] = 'edit'
        # Read snap to grid
        self.settings['snapToGrid'] = self.controls['snapToGrid'].isChecked()
        # Emit object changed signal
        self.signal_object_changed.emit()







class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.resize(1024, 768)
        self.setWindowTitle('Spline Builder')

        splitter = QSplitter(QtCore.Qt.Horizontal)

        self.canvas = splinekernel.CanvasSpline2D()
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