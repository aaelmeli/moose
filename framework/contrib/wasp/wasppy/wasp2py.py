from __future__ import print_function
import subprocess, os, json
from subprocess import Popen, PIPE
import xml.etree.ElementTree as ET

def get_wasp_utility_path(utility_name):
    path = __file__
    if os.name == 'nt' :
        ### for windows intel
        path = os.path.dirname(__file__) + "\\..\\wasputils\\" + utility_name + ".exe"
        if not os.path.isfile(path) :
            ### for windows visual studio
            path = os.path.dirname(__file__) + "\\..\\wasputils\\Release\\" + utility_name + ".exe"
            if not os.path.isfile(path) :
                ### for windows visual studio shared with CMAKE_RUNTIME_OUTPUT_DIRECTORY fixup
                path = os.path.dirname(__file__) + "\\..\\cmake_runtime_output\\Release\\" + utility_name + ".exe"
                if not os.path.isfile(path):
                    path = os.path.dirname(__file__)+"\\..\\bin\\" + utility_name + ".exe"
    else : 
        path = os.path.dirname(__file__) + "/../wasputils/" + utility_name
        if not os.path.isfile(path):
            path = os.path.dirname(__file__)+"/../bin/" + utility_name
    return path

def run_to_dict(cmd):
    """run command that generates json and return the resulting typed dict"""
    proc = Popen(cmd, shell=True, stdout=PIPE)
    json_result = proc.communicate()[0]
    document_dictionary = json.loads(json_result)
    return document_dictionary

def get_xml(input_filepath, type="son"):
    """Utility function for converting definition-less input 
    (typically SON-formatted schema) into XML"""
    utility_name = type+"xml" 
    waspjson = get_wasp_utility_path(utility_name)
    cmd = '"' + waspjson + '" "' + input_filepath + '" --nondec'
    proc = Popen(cmd, shell=True, stdout=PIPE)
    xml_result_string = proc.communicate()[0]
    root = ET.fromstring(xml_result_string)
    return root

def get_json_dict(schema_filepath, input_filepath, ext=""):
    
    if   (ext == 'ddi') or (input_filepath[-4:] == '.ddi') : utility_name = 'ddivalidjson'
    elif (ext=="eddi") : utility_name="eddivalidjson"
    else : utility_name = 'sonvalidjson' 
    
    waspvalidjson = get_wasp_utility_path(utility_name)
    
    cmd = '"' + waspvalidjson + '" "' + schema_filepath + '" "' + input_filepath + '"'
    # print "CMD: ",cmd
    return run_to_dict(cmd)
