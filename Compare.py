#!/usr/bin/python -tt
# Aurora-client Shell
# SAVI Mcgill: Heming Wen, Prabhat Tiwary, Kevin Han, Michael Smith

"""
Command-line interface to the Aurora API
Uses a JSON file for commands
Format:[
         {
          optional:[[oarg1, {attributes}],[oarg2, {attributes}]...], 
          positional:[[parg1, {attributes}], [parg2, {attributes}]...], 
          subargument:[[subarg1, {attributes}, [[osarg1, {attributes}], [osarg2, {attributes}]]], [subarg2, {attributes}]...]
         }
       ]
"""

import argparse
import json
import os
from pprint import pprint
import sys

#from keystoneclient.v2_0 import client as ksclient #commented in order to compile locally

from auroraclient import json_sender
from auroraclient import slice_json_generator

CLIENT_DIR = os.path.dirname(os.path.abspath(__file__))

class AuroraArgumentParser(argparse.ArgumentParser):
    
    def base_parser(self):
        parser = argparse.ArgumentParser(prog='aurora', description='Virtualization and SDI for wireless access points (SAVI Testbed)',
                                         epilog='Created by the SAVI McGill Team')
        
        subparsers = parser.add_subparsers()
        
        # Load the JSON file
        try:
            with open(os.path.join(CLIENT_DIR, 'json/shell.json'), 'r') as JFILE:
                commands = json.load(JFILE)[0]
        except:
            print('Error loading json file!')
            sys.exit(-1)
        
        # Load all optional arguments
        for oarg in commands['optional']:
            parser.add_argument(oarg[0], action=oarg[1]['action'], nargs=oarg[1]['nargs'], default=oarg[1]['default'],
                                choices=oarg[1]['choices'], metavar=oarg[1]['metavar'], help=oarg[1]['help'])
                                
        
        # Load all positional arguments
        for parg in commands['positional']:
            parser.add_argument(parg[0], action=parg[1]['action'], nargs=parg[1]['nargs'], default=parg[1]['default'],
                                choices=parg[1]['choices'], metavar=parg[1]['metavar'], help=parg[1]['help'])
                                
       
        # Load all sub arguments
        for subarg in commands['subargument']:
            temp_parser = subparsers.add_parser(subarg[0], help=subarg[1]['help'])
            temp_parser.add_argument(subarg[0], action=subarg[1]['action'], nargs=subarg[1]['nargs'], default=subarg[1]['default'],
                                     choices=subarg[1]['choices'], metavar=subarg[1]['metavar'])
            
            # Load all optional and positional arguments for the current sub arguemnt
            for osarg in subarg[2]:
                if type(osarg[0]) is list and osarg[1]['action']=='store':
                    temp_parser.add_argument(*osarg[0], action=osarg[1]['action'], nargs=osarg[1]['nargs'], default=osarg[1]['default'],
                                             choices=osarg[1]['choices'], metavar=osarg[1]['metavar'], help=osarg[1]['help'], 
                                             required=osarg[1].get('required', False))
                
                elif type(osarg[0]) is not list and osarg[1]['action']=='store':
                    temp_parser.add_argument(osarg[0], action=osarg[1]['action'], nargs=osarg[1]['nargs'], default=osarg[1]['default'],
                                             choices=osarg[1]['choices'], metavar=osarg[1]['metavar'], help=osarg[1]['help'], 
                                             required=osarg[1].get('required', False))
                
                elif type(osarg[0]) is list and osarg[1]['action']=='store_true':
                    temp_parser.add_argument(*osarg[0], action=osarg[1]['action'], default=osarg[1]['default'], help=osarg[1]['help'], 
                                             required=osarg[1].get('required', False))
                                             
                else:
                    temp_parser.add_argument(osarg[0], action=osarg[1]['action'], default=osarg[1]['default'], help=osarg[1]['help'], 
                                             required=osarg[1].get('required', False))
            if len(subarg) == 4:
                # Positional subargs are present
                for psarg in subarg[3]:
                    if type(psarg[0]) is list and psarg[1]['action']=='store':
                        temp_parser.add_argument(*psarg[0], action=psarg[1]['action'], nargs=psarg[1]['nargs'], default=psarg[1]['default'],
                                                 choices=psarg[1]['choices'], metavar=psarg[1]['metavar'], help=psarg[1]['help'])
                    
                    elif type(psarg[0]) is not list and psarg[1]['action']=='store':
                        temp_parser.add_argument(psarg[0], action=psarg[1]['action'], nargs=psarg[1]['nargs'], default=psarg[1]['default'],
                                                 choices=psarg[1]['choices'], metavar=psarg[1]['metavar'], help=psarg[1]['help'])
                    
                    elif type(psarg[0]) is list and psarg[1]['action']=='store_true':
                        temp_parser.add_argument(*psarg[0], action=psarg[1]['action'], default=psarg[1]['default'], help=psarg[1]['help'])
                                                 
                    else:
                        temp_parser.add_argument(psarg[0], action=psarg[1]['action'], default=psarg[1]['default'], help=psarg[1]['help'])
        
        return parser

class AuroraConsole():

    def main(self, argv):
        if(len(argv) < 2):
            print('Error! Unexpected number of arguments.')
        else:
            function = argv[1] #Used for attrs function call
            parser = AuroraArgumentParser()
            params = vars(parser.base_parser().parse_args(argv[1:]))
            print 'function: %s' % function
            #For add-slice and modify, we need to load the JSON file
            if function == 'ap-slice-create' or function == 'ap-slice-modify':
                if not params['file']:
                    if not params['hint']:   # once the hint tag is also missing, the command is not legit                  
                        print 'Please Specify a file argument!'
                        return
                else:
                    try:
                        JFILE = open(os.path.join(CLIENT_DIR, 'json', params['file'][0]), 'r')
                        #print JFILE
                        file_content = json.load(JFILE)
                        params['file'] = file_content
                        JFILE.close()
                    except:
                        print('Error loading json file!')
                        sys.exit(-1)
            #Authenticate

#            try:
#                authInfo = self.authenticate()
#            except:
#                print 'Invalid Credentials!'
#                sys.exit(-1)       
            if 'hint' in params: #--"hint" token, store params['file'] for the third sent
                #JFILE = open(os.path.join(CLIENT_DIR, 'json/yangwutest.json'), 'r')
                #store = json.load(JFILE)
                #JFILE.close()
                
            #We will send in the following format: {function:"",parameters:""}
            to_send = {
                'function':function,
                'parameters':params,
                'tenant_id':os.environ.get("AURORA_TENANT", -1),
                'project_id':os.environ.get("AURORA_PROJECT", -1),
                'user_id':os.environ.get("AURORA_USER", -1),
            }
            ##FOR DEBUGGING PURPOSES
            print json.dumps(to_send, indent=4)
            ##END DEBUG
            
            if to_send: #--help commands will not start the server
                json_sender.JSONSender().send_json('http://localhost:5554', to_send) # change back to 132.206.206.133:5554

            try:
                if "location" in params['hint'] or "location,slice-load" in params['hint']: # Once the token is 'hint', wait for users' reply
                    exitLoop = False
                    while not exitLoop:
                        print "Please Enter the location where the AP you want to access:"
                        print "(Enter 0 to leave otherwise)"

                        choice = raw_input()
                        if choice == '0':
                            exitLoop = True;
                        elif len(choice) > 0: # Unable to do a logic checking here due to the lack of data at the file
                            print "the location is " + choice

                            ##############Send the information back again to the manager###############
                            params['location'] = choice
                           ## print params
                            to_send = {
                                'function':function,
                                'parameters':params,
                                'tenant_id':os.environ.get("AURORA_TENANT", -1),
                                'project_id':os.environ.get("AURORA_PROJECT", -1),
                                'user_id':os.environ.get("AURORA_USER", -1),
                            }
                            ##FOR DEBUGGING PURPOSES
                            #pprint(to_send)
                            ##END DEBUG
                            
                            if to_send: #--help commands will not start the server
                                message = json_sender.JSONSender().send_json('http://localhost:5554', to_send) # change back to 132.206.206.133:5554
                                self.slice_json_generator = slice_json_generator.SliceJsonGenerator(os.path.join(CLIENT_DIR, 'json/yangwutest.json'),1,1,1); # Initialize the slice_json_generator
                                if 'openflow' in message.lower(): # Restore params['file'] and clean up params['hint'] to create a slice
                                    params['file'] = store
                                    params['hint'] = None
                                    del params['location']
                                    params['ap'] = [message]
                                    print params
                                    to_send = {
                                        'function':function,
                                        'parameters':params,
                                        'tenant_id':os.environ.get("AURORA_TENANT", -1),
                                        'project_id':os.environ.get("AURORA_PROJECT", -1),
                                        'user_id':os.environ.get("AURORA_USER", -1),
                                    }
                                    pprint(to_send)
                                    if to_send: #--help commands will not start the server
                                        message = json_sender.JSONSender().send_json('http://localhost:5554', to_send) # change back to 132.206.206.133:5554
                                    
                                        
                            exitLoop = True;
                        else:
                            exitLoop = True;
                            print "Invalid information"
            except:
                print "Finshed the current statement"
                    
            
        
    def _get_ksclient(self, **kwargs):
        """Get an endpoint and auth token from Keystone.
        :param username: name of user
        :param password: user's password
        :param tenant_id: unique identifier of tenant
        :param tenant_name: name of tenant
        :param auth_url: endpoint to authenticate against
        """
        return ksclient.Client(username=kwargs.get('username'),
                               password=kwargs.get('password'),
                               tenant_id=kwargs.get('tenant_id'),
                               tenant_name=kwargs.get('tenant_name'),
                               auth_url=kwargs.get('auth_url'),
                               cacert=kwargs.get('cacert'),
                               insecure=kwargs.get('insecure'))
    
    def authenticate(self):
        kwargs = {
                    'username': os.getenv('OS_USERNAME'),
                    'password': os.getenv('OS_PASSWORD'),
                    'tenant_id': os.getenv('OS_TENANT_ID'),
                    'tenant_name': os.getenv('OS_TENANT_NAME'),
                    'auth_url': os.getenv('OS_AUTH_URL'),
                    'endpoint_type': os.getenv('OS_ENDPOINT_TYPE'),
                    'cacert': os.getenv('OS_CACERT'),
                    'insecure': False,
                    'region_name': os.getenv('OS_REGION_NAME')
                }
        _ksclient = self._get_ksclient(**kwargs)
        token = _ksclient.auth_token
        return token
        
def main():
    AuroraConsole().main(sys.argv)
        
if __name__ == '__main__':
    main()
