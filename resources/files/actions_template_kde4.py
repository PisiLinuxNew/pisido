#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Licensed under the GNU General Public License, version 3.
# See the file https://www.gnu.org/licenses/gpl-3.0.txt

from pisi.actionsapi import pisitools
from pisi.actionsapi import kde4

def setup():
    kde4.configure()

def build():
    kde4.make()

def install():
    kde4.install()

    pisitools.dodoc("")

