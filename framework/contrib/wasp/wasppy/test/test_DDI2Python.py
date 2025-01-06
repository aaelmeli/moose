from __future__ import print_function
import sys,os
# MAKE SURE THE wasppy MODULE IS IN THE PATH
sys.path.insert(0, os.path.dirname(__file__)+"/..")
import unittest, wasp2py

if os.name == 'nt' :
    schema_filepath = os.path.dirname(__file__)+"\\DDI2Python_schema.sch"
    ddi_input_filepath = os.path.dirname(__file__)+"\\DDI2Python_input.ddi"
else:
    schema_filepath = os.path.dirname(__file__)+"/DDI2Python_schema.sch"
    ddi_input_filepath = os.path.dirname(__file__)+"/DDI2Python_input.ddi"

### obtain pieces of input by name for convenience
document = wasp2py.get_json_dict(schema_filepath,ddi_input_filepath)

# class for unit testing
class TestDDI2Python(unittest.TestCase):

    def test_application(self):
        self.assertEqual(document['application']['value'],               '/path/to/executable arg1 arg2 arg3')
        self.assertEqual(document['application']['input_file']['value'], 'filename.i')

    def test_extract_from(self):
        for idx, extract in enumerate(document['extract_from']):
            if idx == 0:
                self.assertEqual(extract['value'], 'filename_1.out')
            if idx == 1:
                self.assertEqual(extract['value'], 'filename_2.out')

    def test_columns(self):
        for idx, extract in enumerate(document['extract_from']):
            if idx == 1:
                for idy, fnd in enumerate(extract['find']):
                    if idy == 3:
                        for idz, col in enumerate(fnd['column']):
                            if idz == 0:
                                for val_id, colval in enumerate(col['value']):
                                    if val_id == 0:
                                        self.assertEqual(colval, 3)
                                    if val_id == 1:
                                        self.assertEqual(colval, 4)
                            if idz == 1:
                                for val_id, colval in enumerate(col['value']):
                                    if val_id == 0:
                                        self.assertEqual(colval, 5)
                                    if val_id == 1:
                                        self.assertEqual(colval, 6)
                                    if val_id == 2:
                                        self.assertEqual(colval, 2)

if __name__ == '__main__':
    unittest.main()



