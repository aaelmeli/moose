from __future__ import print_function
import sys,os
# MAKE SURE THE waspdrive MODULE IS IN THE PATH
sys.path.insert(0, os.path.dirname(__file__)+"/..")
import unittest, waspdrive

# class for unit testing
class TestDrive(unittest.TestCase):
    document = None

    def test_driver_scheduled(self):
        try:
          app_driver_input = os.path.dirname(__file__)+"/scheduled_test.drive"
          # Re-use the 'using' input
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

