#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

from FileTester import FileTester
from TestHarness import util
import os
from TestHarness.XMLDiffer import XMLDiffer

class XMLDiff(FileTester):

    @staticmethod
    def validParams():
        params = FileTester.validParams()
        params.addRequiredParam('xmldiff',   [], "A list of XML files to compare.")
        params.addParam('gold_dir',      'gold', "The directory where the \"golden standard\" files reside relative to the TEST_DIR: (default: ./gold/)")
        params.addParam('abs_zero',       1e-10, "Absolute zero cutoff used in exodiff comparisons.")
        params.addParam('rel_err',       5.5e-6, "Relative error value used in exodiff comparisons.")
        params.addParam('ignored_attributes',  [], "Ignore e.g. type and/or version in sample XML block <VTKFile type=\"Foo\" version=\"0.1\">")

        return params

    def __init__(self, name, params):
        FileTester.__init__(self, name, params)

    def prepare(self, options):
        if self.specs['delete_output_before_running'] == True:
            util.deleteFilesAndFolders(self.getTestDir(), self.specs['xmldiff'])

    def processResults(self, moose_dir, options, output):
        output = FileTester.processResults(self, moose_dir, options, output)
        specs = self.specs

        if self.isFail() or specs['skip_checks']:
            return output

        # Don't Run XMLDiff on Scaled Tests
        if options.scaling and specs['scale_refine']:
            return output

        # Loop over every file
        for file in specs['xmldiff']:
            # Perform diff
            for file in self.specs['xmldiff']:
                gold = os.path.join(self.getTestDir(), specs['gold_dir'], file)
                test = os.path.join(self.getTestDir(), file)

                # We always ignore the header_type attribute, since it was
                # introduced in VTK 7 and doesn't seem to be important as
                # far as Paraview is concerned.
                specs['ignored_attributes'].append('header_type')

                differ = XMLDiffer(gold, test, abs_zero=specs['abs_zero'], rel_tol=specs['rel_err'], ignored_attributes=specs['ignored_attributes'])

                # Print the results of the XMLDiff whether it passed or failed.
                output += differ.message() + '\n'

                if differ.fail():
                    self.setStatus(self.diff, 'XMLDIFF')
                    break

        return output
