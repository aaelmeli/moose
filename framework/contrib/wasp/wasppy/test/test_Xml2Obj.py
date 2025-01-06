from __future__ import print_function, absolute_import, unicode_literals
import subprocess, os, sys
# MAKE SURE THE xml2obj MODULE IS IN THE PATH
sys.path.insert(0, os.path.dirname(__file__)+"/..")
sys.path.append("..")
import xml2obj, wasp2py
import unittest

### convert son file to xml stream and create python data structure
sonvalidxml = wasp2py.get_wasp_utility_path("sonvalidxml")

if os.name == 'nt':    
    schema_filepath = os.path.dirname(__file__)+"\\schema.sch"
    son_input_filepath = os.path.dirname(__file__)+"\\input.son"
else:    
    schema_filepath = os.path.dirname(__file__)+"/schema.sch"
    son_input_filepath = os.path.dirname(__file__)+"/input.son"
cmd = sonvalidxml+' '+schema_filepath+' '+son_input_filepath+" --xml"
xmlresult = subprocess.check_output(cmd, shell=True)
if isinstance( xmlresult, bytes ):
    xmlresult = xmlresult.decode( 'utf-8' )


### obtain pieces of input by name for convenience
document = xml2obj.xml2obj(xmlresult)
pytest = document.pytest
object_one = pytest.object_one
color = object_one.color
foos = object_one.foos
bars = object_one.bars
object_two = pytest.object_two
circle   = object_two.circle
square   = object_two.square
triangle = object_two.square.triangle
pentagon = object_two.square.pentagon
    ### class for unit testing

class TestArcPy(unittest.TestCase):

    def test_color(self):
        self.assertEqual(str(color.value), 'blue')

    def test_foo_names_temps(self):
        for idx, foo in enumerate(foos.foo):
            if   idx == 0:
                self.assertEqual(str(foo.id),     'foo_name1')
                self.assertEqual(str(foo.numval.value), '101')
            elif idx == 1:
                self.assertEqual(str(foo.id),     'foo_name2')
                self.assertEqual(str(foo.numval.value), '102')
            elif idx == 2:
                self.assertEqual(str(foo.id),     'foo_name3')
                self.assertEqual(str(foo.numval.value), '103')
            elif idx == 3:
                self.assertEqual(str(foo.id),     'foo_name4')
                self.assertEqual(str(foo.numval.value), '104')
            elif idx == 4:
                self.assertEqual(str(foo.id),     'foo_name5')
                self.assertEqual(str(foo.numval.value), '105')
            elif idx == 5:
                self.assertEqual(str(foo.id),     'foo_name6')
                self.assertEqual(str(foo.numval.value), '106')

    def test_foo_fffs_fnum(self):
        for idx, foo in enumerate(foos.foo):
            if str(foo.fffs) != 'None':
                for idy, fffs in enumerate(foo.fffs):
                    if str(fffs.f_num) != 'None':
                        for idz, f_num in enumerate(fffs.f_num):
                            if  idy == 0 and idz == 0:
                                self.assertEqual(str(f_num.id),  'ac226')
                                self.assertEqual(str(f_num.value), '0.7')
                            if  idy == 0 and idz == 1:
                                self.assertEqual(str(f_num.id),  'ac228')
                                self.assertEqual(str(f_num.value), '0.2')
                            if  idy == 0 and idz == 2:
                                self.assertEqual(str(f_num.id),  'ac229')
                                self.assertEqual(str(f_num.value), '0.1')
                            if  idy == 1 and idz == 0:
                                self.assertEqual(str(f_num.id),  'ac221')
                                self.assertEqual(str(f_num.value), '0.3')
                            if  idy == 1 and idz == 1:
                                self.assertEqual(str(f_num.id),  'ac224')
                                self.assertEqual(str(f_num.value), '0.3')
                            if  idy == 1 and idz == 2:
                                self.assertEqual(str(f_num.id),  'ac224')
                                self.assertEqual(str(f_num.value), '0.4')

    def test_bars_typethree_y(self):
        typethree_y_count = 0
        for idx, typethree in enumerate(bars.typethree):
            if str(typethree.aaa.value) == 'y':
                typethree_y_count += 1
                if typethree_y_count == 1:
                    self.assertEqual(str(typethree.id), 'typethree_name3' )
                    self.assertEqual(str(typethree.bbb.value),      '0.1' )
                    self.assertEqual(str(typethree.ccc),           'None' )
                if typethree_y_count == 2:
                    self.assertEqual(str(typethree.id), 'typethree_name4' )
                    self.assertEqual(str(typethree.bbb),           'None' )
                    self.assertEqual(str(typethree.ccc.value),     '0.1' )

    def test_calculations(self):
        self.assertEqual(str(circle.name.value),        'john' )
        self.assertEqual(str(circle.phone.value),    '5551224' )
        self.assertEqual(str(square.job.value),       'doctor' )
        self.assertEqual(str(square.sport.value),     'tennis' )
        self.assertEqual(str(triangle.sides.value),        '3' )
        self.assertEqual(str(triangle.type.value), 'isosceles' )
        self.assertEqual(str(pentagon.rise.value),      '76.3' )
        self.assertEqual(str(pentagon.run.value),       '19.3' )
        for idx, foo_name_choice in enumerate(pentagon.foo_name_choices.value):
            if   idx == 0:
                self.assertEqual(str(foo_name_choice), 'foo_name1')
            elif idx == 1:
                self.assertEqual(str(foo_name_choice), 'foo_name2')

if __name__ == '__main__':
    unittest.main()



