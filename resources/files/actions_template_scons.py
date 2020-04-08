#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Licensed under the GNU General Public License, version 3.
# See the file https://www.gnu.org/licenses/gpl-3.0.txt

from pisi.actionsapi import pisitools
from pisi.actionsapi import scons

def setup():
    scons.configure()

def build():
    scons.make()

def install():
    scons.install()

