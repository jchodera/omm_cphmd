/** amberparm.h
  * 
  * This file contains the functionality for reading Amber topology files and
  * instantiating molecular structure classes based on them.
  *
  */

#ifndef AMBERPARM_H
#define AMBERPARM_H

#include <cmath>
#include <string>
#include <set>

#include "topology.h"
#include "readparm.h"

#include "OpenMM.h"

namespace Amber {

class AmberParm {
    public:
        /** Reads an Amber topology file and sets up data structures defining
          * the topology and parameters. If a filename is passed to the
          * constructor, it is read.
          */
        AmberParm(void) : ifbox_(0) {}
        AmberParm(std::string const& filename);
        AmberParm(const char* filename);

        // Iterators
        typedef AtomList::const_iterator atom_iterator;
        typedef BondList::const_iterator bond_iterator;
        typedef AngleList::const_iterator angle_iterator;
        typedef DihedralList::const_iterator dihedral_iterator;

        atom_iterator AtomBegin(void) const {return atoms_.begin();}
        atom_iterator AtomEnd(void) const {return atoms_.end();}
        bond_iterator BondBegin(void) const {return bonds_.begin();}
        bond_iterator BondEnd(void) const {return bonds_.end();}
        angle_iterator AngleBegin(void) const {return angles_.begin();}
        angle_iterator AngleEnd(void) const {return angles_.end();}
        dihedral_iterator DihedralBegin(void) const {return dihedrals_.begin();}
        dihedral_iterator DihedralEnd(void) const {return dihedrals_.end();}

        /* To add parameters. They can either be added using the parameter type
         * directly or by providing the atom indexes and the parameters. If the
         * indexes of the provided atoms are not sequential, an AmberParmError
         * will be thrown. If atom properties are provided, the indexes will be
         * assigned sequentially.
         *
         * If any bonds, angles, or dihedrals are assigned with atom indexes
         * out of range of the main atom list, an AmberParmError will be thrown
         * (meaning that atoms MUST be added first).
         */
        void addAtom(Atom& new_atom);
        void addAtom(std::string const& name, std::string const& type,
                     int element, double mass, double charge, double lj_rad,
                     double lj_eps, double gb_rad, double gb_screen);
        void addAtom(const char *name, const char *type, int element,
                     double mass, double charge, double lj_rad, double lj_eps,
                     double gb_rad, double gb_screen);

        void addBond(Bond& new_bond);
        void addBond(int i, int j, double kf, double req);

        void addAngle(Angle& new_angle);
        void addAngle(int i, int j, int k, double kf, double theteq);

        void addDihedral(Dihedral& new_dihedral);
        void addDihedral(int i, int j, int k, int l, double kf, double phase,
                         int periodicity, double scee, double scnb,
                         bool ignore_end);

        // Getters for the parameter types -- cannot modify them
        AtomList Atoms(void) const {return atoms_;}
        BondList Bonds(void) const {return bonds_;}
        AngleList Angles(void) const {return angles_;}
        DihedralList Dihedrals(void) const {return dihedrals_;}
        std::vector<int> ResiduePointers(void) const {return residue_pointers_;}
        std::vector<std::string> ResidueLabels(void) const {
            return residue_labels_;
        }
        /** Returns true if 2 atoms are excluded or false if they are not (note,
          * exceptions, like the 1-4 interactions, do NOT count as exclusions)
          */
        bool isExcluded(int i, int j) const {
            if (i == j) return true;
            if (i < j) {
                return exclusion_list_[i].count(j) > 0;
            }
            return exclusion_list_[j].count(i) > 0;
        }

        /** Returns true if the system uses periodic bondaries and false
          * otherwise
          */
        bool isPeriodic(void) const {return ifbox_ > 0;}
        int IfBox(void) const {return ifbox_;}

        void printExclusions(int i);

        /// Read a prmtop file and instantiate a structure from it.
        void rdparm(std::string const& filename);
        void rdparm(const char* filename);

        /// Create an OpenMM System
        OpenMM::System* createSystem(
            OpenMM::NonbondedForce::NonbondedMethod nonbondedMethod=OpenMM::NonbondedForce::NoCutoff,
            double nonbondedCutoff=10.0,
            std::string constraints=std::string("None"),
            bool rigidWater=false,
            std::string implicitSolvent=std::string("None"),
            double implicitSolventKappa=0.0,
            double implicitSolventSaltConc=0.0,
            double temperature=298.15,
            double soluteDielectric=1.0,
            double solventDielectric=78.5,
            bool removeCMMotion=true,
            double ewaldErrorTolerance=0.0005,
            bool flexibleConstraints=true);

    private:
        int ifbox_;
        AtomList atoms_;
        BondList bonds_;
        AngleList angles_;
        DihedralList dihedrals_;
        std::vector<int> residue_pointers_;
        std::vector<std::string> residue_labels_;
        std::vector<std::set<int> > exclusion_list_;
};

enum ForceGroup {BOND_FORCE_GROUP=0, ANGLE_FORCE_GROUP, DIHEDRAL_FORCE_GROUP,
                 NONBONDED_FORCE_GROUP};

}; // namespace Amber

#endif /* AMBERPARM_H */