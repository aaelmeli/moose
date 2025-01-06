from __future__ import print_function
import sys,os
# MAKE SURE THE waspdrive MODULE IS IN THE PATH
sys.path.insert(0, os.path.dirname(__file__)+"/..")
import unittest, waspdrive

# class for unit testing
class TestDrive(unittest.TestCase):
    document = None
    def test_first_n_lines(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_line=waspdrive.first_n_lines(app_file,3)
          self.assertEqual(str_line, 
            ["application 'python fake_driver_app.py'\n","    input_file 'fake_app_input'\n","    input_tmpl 'my_fake_app.tmpl'\n"], "Line doesn't match")
        finally:
           os.chdir("../") 
 
    def test_last_n_lines(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_line=waspdrive.last_n_lines(app_file,3)
          self.assertEqual(str_line, 
            ["        column 2 delimiter ' ' as 'x1' \n", "        column 3 delimiter ' ' as 'x2' \n","        column 4 delimiter ' ' as 'x3'\n"], "Line doesn't match")
        finally:
           os.chdir("../") 
        
    def test_first_n_line(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_line=waspdrive.first_n_line(app_file,3)
          self.assertEqual(str_line, 
            "    input_tmpl 'my_fake_app.tmpl'\n", "Line doesn't match")
        finally:
           os.chdir("../") 

    def test_last_n_line(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_line=waspdrive.last_n_line(app_file,3)
          self.assertEqual(str_line, 
            "        column 2 delimiter ' ' as 'x1' \n", 
            "Line doesn't match")
        finally:
           os.chdir("../") 

    def test_between_lines(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_lines=waspdrive.between_lines(app_file,2,5)
          self.assertEqual(str_lines, 
            "    input_file 'fake_app_input'\n"+
            "    input_tmpl 'my_fake_app.tmpl'\n"+
            "extract_from 'fake_driver_app.csv'\n"+
            "    find last_line 1\n", 
            "Lines don't match")
        finally:
           os.chdir("../") 

    def test_between_patterns(self):
        try:
          app_file= os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_lines=waspdrive.between_patterns(app_file,"column",
            "x[0-9]{1}'")
          self.assertEqual(str_lines," 2 delimiter ' ' as '\n 3 delimiter ' ' as '\n 4 delimiter ' ' as '\n",
            str_lines)
        finally:
           os.chdir("../") 

    def test_grep_string(self):
        try:
          app_file=os.path.dirname(__file__)+"/last_line.driver"
          os.chdir("test")
          str_lines=waspdrive.grep_string(app_file,"column")
          self.assertEqual(str_lines,
            "        column 2 delimiter ' ' as 'x1' \n"+
            "        column 3 delimiter ' ' as 'x2' \n"+
            "        column 4 delimiter ' ' as 'x3'\n",
            "Lines don't match")
        finally:
           os.chdir("../") 

    def test_between_patterns_driver(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/between_and_the_others.driver"
          app_json_parameters = os.path.dirname(__file__)+"/between_and_the_others.json"

          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)
          self.assertIsNotNone(self.document, "Failed to acquire document!")

          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")
    
          results = waspdrive.extract_results(self.document)
          self.assertListEqual(
            [64.0, 27.5, # x1, x2
             16.5, 0.5, # 
             23.25, 46.0, # 
             10.0, 64.0,#v1, v2
             20.75, 16.5,
             15.0, 23.25,
             'x:', '3.75', #column 1, 2 of the whole file
             'v:', '40.5', 
             'x:', '2.5', 
             'Table', 'Header',
             '5.25', '10.0', 
             '28.5', '20.75', 
             '39.5', '15.0',
             'Table', 'Footer',
             '23.0', '42.0',
             'Table Footer\n23.0 42.0 46.5 59.25 \n', # last 2 lines
             'x: 3.75 58.5 37.0 \nv: 40.5 4.5 \n', # first 2 lines
             '5.25 10.0 64.0 27.5 \n28.5 20.75 16.5 0.5 \n39.5 15.0 23.25 46.0 ', #between table tags 
             'x: 3.75 58.5 37.0 \nx: 2.5 48.0 32.0 ' # pattern 'x'       
          ], results, results)        
        finally:
           os.chdir("../") 

    def test_first_line_driver(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/first_line.driver"
          app_json_parameters = os.path.dirname(__file__)+"/first_line.json"
          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)
          self.assertIsNotNone(self.document, "Failed to acquire document!")
        
          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")
        
          results = waspdrive.extract_results(self.document)
          self.assertListEqual([14.5,14.75,83.0], results, results)
        finally:
           os.chdir("../") 
    
    def test_last_line_driver(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/last_line.driver"
          app_json_parameters = os.path.dirname(__file__)+"/last_line.json"
        
          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)
          self.assertIsNotNone(self.document, "Failed to acquire document!")

          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")
    
          results = waspdrive.extract_results(self.document)
          self.assertListEqual([10.5, 51.0, 50.25], results)
        finally:
           os.chdir("../") 
        

    def test_pattern_column_driver(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/pattern_column.driver"
          app_json_parameters = os.path.dirname(__file__)+"/pattern_column.json"

          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)
          self.assertIsNotNone(self.document, "Failed to acquire document!")

          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")
    
          results = waspdrive.extract_results(self.document)
          self.assertListEqual(
            [58.5, 37.0, # x
             48.0, 32.0, # x
             40.5, 4.5, # v             
          ], results, results)        
        finally:
           os.chdir("../") 

    def test_pattern_columns_driver(self):
        """Test finding a pattern and extracting multiple columns"""
        try:
          app_driver_input = os.path.dirname(__file__)+"/pattern_columns.driver"
          app_json_parameters = os.path.dirname(__file__)+"/pattern_column.json" # original input is good enough

          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)

          self.assertIsNotNone(self.document, "Failed to acquire document!")

          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")

          results = waspdrive.extract_results(self.document)
          self.assertListEqual(
            [58.5, 37.0, # x
             48.0, 32.0, # x
             40.5, 4.5, # v
          ], results, results)
        finally:
           os.chdir("../") 

    def test_using_driver(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/using.driver"
          app_json_parameters = os.path.dirname(__file__)+"/using.json"

          os.chdir("test")
          ### obtain pieces of input by name for convenience
          self.document = waspdrive.process_drive_input(app_driver_input)
          self.assertIsNotNone(self.document, "Failed to acquire document!")

          rtncode = waspdrive.run_external_app(self.document, app_json_parameters)
          self.assertEqual(0, rtncode, "External application return code is not 0!")
    
          results = waspdrive.extract_results(self.document)
          self.assertListEqual(
            [123456789], results) 
        finally:
           os.chdir("../") 

if __name__ == '__main__':
    unittest.main()

