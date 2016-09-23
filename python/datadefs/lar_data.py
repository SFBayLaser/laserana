__author__ = 'matthias'

from datadefs.laserdef import *
from datadefs.recobhits import *
import numpy as np
import root_numpy as rn


class LarData():
    def __init__(self, filename):
        self.file = filename
        self.datatypes = ["laser", "hits"]

        self.id = None

        self.laser = None
        self.hits = None

        self.n_entries = None

    def read_laser(self):
        laserdefs = Laseref()
        tree = laserdefs.tree
        branches = [laserdefs.pos(), laserdefs.dir(), laserdefs.power(), laserdefs.id()]
        self.laser = self.read(tree, branches)

    def read_hits(self, planes=None):
        if planes is None:
            recodef = RecobHits(plane="Y")
        else:
            recodef = RecobHits(plane=planes)
        hit_branches = [recodef.channel(), recodef.peak_time(), recodef.peak_amplitude(), recodef.start_tick(),
                        recodef.end_tick()]
        self.hits = self.read(recodef.tree, hit_branches)

        # hits.dtype.names = ["channel", "peak_tick", "peak_amp", "start_tick", "end_tick"]

    def read(self, tree, branches):
        branch = [item for sublist in branches for item in sublist if isinstance(sublist, list)] \
                 + [item for item in branches if isinstance(item, str)] # handling lists and strings only

        data = rn.root2array(self.file, treename=tree, branches=branch).view(np.recarray)

        if self.n_entries is None:
            self.n_entries = len(data)

        return data

    def get_laser(self, event):
        if type(event) is not int:
            raise ValueError("only integers allowed as event number")

        data =  self.laser[event]
        data.dtype.names= ("pos_x", "pos_y", "pos_z", "dir_x", "dir_y", "dir_z", "power", "id")

        return data

    def get_hits(self, event):
        if type(event) is not int:
            raise ValueError("only integers allowed as event number")

        data = self.hits[event]
        data.dtype.names = ("channel", "tick", "peak_amp", "start_tick", "end_tick")

        return data

    def get_info(self):
        trees = rn.list_trees(self.file)
        for tree in trees:
            print str.capitalize(tree) + ":"
            print rn.list_branches(self.file, treename=tree)