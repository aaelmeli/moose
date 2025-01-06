from __future__ import print_function
import sys, os
# MAKE SURE THE wasppy MODULE IS IN THE PATH
sys.path.insert(0, os.path.dirname(__file__)+"/..")
import unittest, wasp2py






if os.name == 'nt' :
    schema_filepath = os.path.dirname(__file__)+"\\schema.sch"
    son_input_filepath = os.path.dirname(__file__)+"\\input.son"
else:
    schema_filepath = os.path.dirname(__file__)+"/schema.sch"
    son_input_filepath = os.path.dirname(__file__)+"/input.son"





### obtain pieces of input by name for convenience
document = wasp2py.get_json_dict(schema_filepath,son_input_filepath)
pytest = document['pytest']
object_one = pytest['object_one']
color = object_one['color']
foos = object_one['foos']
bars = object_one['bars']
object_two = pytest['object_two']
circle   = object_two['circle']
square   = object_two['square']
triangle = object_two['square']['triangle']
pentagon = object_two['square']['pentagon']
    ### class for unit testing

class TestJson2Python(unittest.TestCase):

    def test_color(self):
        self.assertEqual(color['value'], 'blue')

    def test_foo_names_temps(self):
        for idx, foo in enumerate(foos['foo']):
            if   idx == 0:
                self.assertEqual(foo['_id'],       'foo_name1')
                self.assertEqual(foo['numval']['value'],   101)
            elif idx == 1:
                self.assertEqual(foo['_id'],       'foo_name2')
                self.assertEqual(foo['numval']['value'],   102)
            elif idx == 2:
                self.assertEqual(foo['_id'],       'foo_name3')
                self.assertEqual(foo['numval']['value'],   103)
            elif idx == 3:
                self.assertEqual(foo['_id'],       'foo_name4')
                self.assertEqual(foo['numval']['value'],   104)
            elif idx == 4:
                self.assertEqual(foo['_id'],       'foo_name5')
                self.assertEqual(foo['numval']['value'],   105)
            elif idx == 5:
                self.assertEqual(foo['_id'],       'foo_name6')
                self.assertEqual(foo['numval']['value'],   106)

    def test_foo_fffs_fnum(self):
        for idx, foo in enumerate(foos['foo']):
            if 'fffs' in foo:
                for idy, fffs in enumerate(foo['fffs']):
                    if 'f_num' in fffs:
                        for idz, f_num in enumerate(fffs['f_num']):
                            if  idy == 0 and idz == 0:
                                self.assertEqual(f_num['_id'], 'ac226')
                                self.assertEqual(f_num['value'],   0.7)
                            if  idy == 0 and idz == 1:
                                self.assertEqual(f_num['_id'], 'ac228')
                                self.assertEqual(f_num['value'],   0.2)
                            if  idy == 0 and idz == 2:
                                self.assertEqual(f_num['_id'], 'ac229')
                                self.assertEqual(f_num['value'],   0.1)
                            if  idy == 1 and idz == 0:
                                self.assertEqual(f_num['_id'], 'ac221')
                                self.assertEqual(f_num['value'],   0.3)
                            if  idy == 1 and idz == 1:
                                self.assertEqual(f_num['_id'], 'ac224')
                                self.assertEqual(f_num['value'],   0.3)
                            if  idy == 1 and idz == 2:
                                self.assertEqual(f_num['_id'], 'ac224')
                                self.assertEqual(f_num['value'],   0.4)

    def test_bars_typethree_y(self):
        typethree_y_count = 0
        for idx, typethree in enumerate(bars['typethree']):
            if typethree['aaa']['value'] == 'y':
                typethree_y_count += 1
                if typethree_y_count == 1:
                    self.assertEqual(typethree['_id'], 'typethree_name3')
                    self.assertEqual(typethree['bbb']['value'],      0.1)
                    self.assertFalse('ccc' in typethree                 )
                if typethree_y_count == 2:
                    self.assertEqual(typethree['_id'], 'typethree_name4')
                    self.assertFalse('bbb' in typethree                 )
                    self.assertEqual(typethree['ccc']['value'],      0.1)

    def test_calculations(self):
        self.assertEqual(circle['name']['value'],        'john')
        self.assertEqual(circle['phone']['value'],      5551224)
        self.assertEqual(square['job']['value'],       'doctor')
        self.assertEqual(square['sport']['value'],     'tennis')
        self.assertEqual(triangle['sides']['value'],          3)
        self.assertEqual(triangle['type']['value'], 'isosceles')
        self.assertEqual(pentagon['rise']['value'],        76.3)
        self.assertEqual(pentagon['run']['value'],         19.3)
        for idx, foo_name_choice in enumerate(pentagon['foo_name_choices']['value']):
            if   idx == 0:
                self.assertEqual(foo_name_choice, 'foo_name1')
            elif idx == 1:
                self.assertEqual(foo_name_choice, 'foo_name2')

if __name__ == '__main__':
    unittest.main()



