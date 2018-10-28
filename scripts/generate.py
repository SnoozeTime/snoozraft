import sys
import argparse
import yaml
import jinja2
import shutil
import os

MSG_FOLDER = 'generated/messages'
GENERATED_FOLDER = 'generated'
TEMPLATES_FOLDER = 'templates'

class Message:
    def __init__(self, yaml_dict):
        self.name = yaml_dict['name']
        self.args = yaml_dict['elements']
        self.message_type = yaml_dict['msg_type']

def load_template(tmpl_name):
    with open("{}/{}".format(TEMPLATES_FOLDER, tmpl_name)) as f:
        return jinja2.Template(f.read())


def generate_class(t: jinja2.Template, msg: Message):
    filename = '{0}/{1}Message.h'.format(MSG_FOLDER,
                                         msg.name)

    with open(filename, 'w') as f:
        f.write(t.render(msg=msg))


def main(msgs):

    print("Will process {}".format(msgs))
    msg_template = load_template('msg_template.cpp')
    include_header_template = load_template('include_template.h')
    include_impl_template = load_template('include_template.cpp')
    msg_handler_template = load_template('msg_handler.h')
    with open(msgs, 'r') as f:
        yml = yaml.load(f)
        msgs = [Message(x) for x in yml['messages']]

        for msg in msgs:
            generate_class(msg_template, msg)

        with open('{}/message.h'.format(GENERATED_FOLDER), 'w') as g:
            g.write(include_header_template.render(enums=[x.message_type for x in msgs]))

        with open('{}/message.cc'.format(GENERATED_FOLDER), 'w') as h:
            h.write(include_impl_template.render(msgs=msgs))

        with open('{}/msg_handler.h'.format(GENERATED_FOLDER), 'w') as k:
            k.write(msg_handler_template.render(classes=msgs))

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("msgs", help="Message specification")
    args = parser.parse_args()

    if not args.msgs:
        sys.exit(1)
    main(args.msgs)
