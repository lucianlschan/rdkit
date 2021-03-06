//
//  Copyright (C) 2019 Greg Landrum
//
//   @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//

#define NO_IMPORT_ARRAY
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <string>

// ours
#include <GraphMol/RDKitBase.h>
#include <GraphMol/SubstanceGroup.h>
#include "props.hpp"

namespace python = boost::python;

namespace RDKit {

namespace {
SubstanceGroup *getMolSubstanceGroupWithIdx(ROMol &mol, unsigned int idx) {
  auto &sgs = getSubstanceGroups(mol);
  if (idx >= sgs.size()) {
    throw_index_error(idx);
  }
  return &(sgs[idx]);
}

std::vector<SubstanceGroup> getMolSubstanceGroups(ROMol &mol) {
  return getSubstanceGroups(mol);
}
void clearMolSubstanceGroups(ROMol &mol) {
  std::vector<SubstanceGroup> &sgs = getSubstanceGroups(mol);
  sgs.clear();
}

SubstanceGroup *createMolSubstanceGroup(ROMol &mol, std::string type) {
  SubstanceGroup sg(&mol, type);
  addSubstanceGroup(mol, sg);
  return &(getSubstanceGroups(mol).back());
}

void addBracketHelper(SubstanceGroup &self, python::object pts) {
  unsigned int sz = python::extract<unsigned int>(pts.attr("__len__")());
  if (sz != 2 && sz != 3)
    throw_value_error("pts object have a length of 2 or 3");

  SubstanceGroup::Bracket bkt;
  python::stl_input_iterator<RDGeom::Point3D> beg(pts);
  unsigned int i = 0;
  for (unsigned int i = 0; i < sz; ++i) {
    bkt[i] = *beg;
    ++beg;
  }
  self.addBracket(bkt);
}

}  // namespace

std::string sGroupClassDoc =
    "A collection of atoms and bonds with associated properties\n";

struct sgroup_wrap {
  static void wrap() {
    // register the vector_indexing_suite for SubstanceGroups
    // if it hasn't already been done.
    // logic from https://stackoverflow.com/a/13017303
    boost::python::type_info info =
        boost::python::type_id<std::vector<RDKit::SubstanceGroup>>();
    const boost::python::converter::registration *reg =
        boost::python::converter::registry::query(info);
    if (reg == NULL || (*reg).m_to_python == NULL) {
      python::class_<std::vector<RDKit::SubstanceGroup>>("SubstanceGroup_VECT")
          .def(python::vector_indexing_suite<
               std::vector<RDKit::SubstanceGroup>>());
    }

    python::class_<SubstanceGroup, boost::shared_ptr<SubstanceGroup>>(
        "SubstanceGroup", sGroupClassDoc.c_str(), python::no_init)
        .def("GetOwningMol", &SubstanceGroup::getOwningMol,
             "returns the molecule owning this SubstanceGroup",
             python::return_internal_reference<>())
        .def("GetIndexInMol", &SubstanceGroup::getIndexInMol,
             "returns the index of this SubstanceGroup in the owning "
             "molecule's list.")
        .def(
            "GetAtoms", &SubstanceGroup::getAtoms,
            "returns a list of the indices of the atoms in this SubstanceGroup",
            python::return_value_policy<python::copy_const_reference>())
        .def("GetParentAtoms", &SubstanceGroup::getParentAtoms,
             "returns a list of the indices of the parent atoms in this "
             "SubstanceGroup",
             python::return_value_policy<python::copy_const_reference>())
        .def(
            "GetBonds", &SubstanceGroup::getBonds,
            "returns a list of the indices of the bonds in this SubstanceGroup",
            python::return_value_policy<python::copy_const_reference>())
        .def("AddAtomWithIdx", &SubstanceGroup::addAtomWithIdx)
        .def("AddBondWithIdx", &SubstanceGroup::addBondWithIdx)
        .def("AddParentAtomWithIdx", &SubstanceGroup::addParentAtomWithIdx)
        .def("AddAtomWithBookmark", &SubstanceGroup::addAtomWithBookmark)
        .def("AddParentAtomWithBookmark",
             &SubstanceGroup::addParentAtomWithBookmark)
        .def("AddCState", &SubstanceGroup::addCState)
        .def("AddBondWithBookmark", &SubstanceGroup::addBondWithBookmark)
        .def("AddAttachPoint", &SubstanceGroup::addAttachPoint)
        .def("AddBracket", addBracketHelper)

        .def("SetProp",
             (void (RDProps::*)(const std::string &, std::string, bool) const) &
                 SubstanceGroup::setProp<std::string>,
             (python::arg("self"), python::arg("key"), python::arg("val"),
              python::arg("computed") = false),
             "sets the value of a particular property")
        .def("SetDoubleProp",
             (void (RDProps::*)(const std::string &, double, bool) const) &
                 SubstanceGroup::setProp<double>,
             (python::arg("self"), python::arg("key"), python::arg("val"),
              python::arg("computed") = false),
             "sets the value of a particular property")
        .def("SetIntProp",
             (void (RDProps::*)(const std::string &, int, bool) const) &
                 SubstanceGroup::setProp<int>,
             (python::arg("self"), python::arg("key"), python::arg("val"),
              python::arg("computed") = false),
             "sets the value of a particular property")
        .def(
            "SetUnsignedProp",
            (void (RDProps::*)(const std::string &, unsigned int, bool) const) &
                SubstanceGroup::setProp<unsigned int>,
            (python::arg("self"), python::arg("key"), python::arg("val"),
             python::arg("computed") = false),
            "sets the value of a particular property")
        .def("SetBoolProp",
             (void (RDProps::*)(const std::string &, bool, bool) const) &
                 SubstanceGroup::setProp<bool>,
             (python::arg("self"), python::arg("key"), python::arg("val"),
              python::arg("computed") = false),
             "sets the value of a particular property")
        .def("HasProp",
             (bool (RDProps::*)(const std::string &) const) &
                 SubstanceGroup::hasProp,
             "returns whether or not a particular property exists")
        .def("GetProp",
             (std::string(RDProps::*)(const std::string &) const) &
                 SubstanceGroup::getProp<std::string>,
             "returns the value of a particular property")
        .def("GetIntProp",
             (int (RDProps::*)(const std::string &) const) &
                 SubstanceGroup::getProp<int>,
             "returns the value of a particular property")
        .def("GetUnsignedProp",
             (unsigned int (RDProps::*)(const std::string &) const) &
                 SubstanceGroup::getProp<unsigned int>,
             "returns the value of a particular property")
        .def("GetDoubleProp",
             (double (RDProps::*)(const std::string &) const) &
                 SubstanceGroup::getProp<double>,
             "returns the value of a particular property")
        .def("GetBoolProp",
             (bool (RDProps::*)(const std::string &) const) &
                 SubstanceGroup::getProp<bool>,
             "returns the value of a particular property")
        .def("GetPropNames", &SubstanceGroup::getPropList,
             (python::arg("self"), python::arg("includePrivate") = false,
              python::arg("includeComputed") = false),
             "Returns a list of the properties set on the SubstanceGroup.\n\n")
        .def("GetPropsAsDict", GetPropsAsDict<SubstanceGroup>,
             (python::arg("self"), python::arg("includePrivate") = true,
              python::arg("includeComputed") = true),
             "Returns a dictionary of the properties set on the "
             "SubstanceGroup.\n"
             " n.b. some properties cannot be converted to python types.\n");
    python::def("GetMolSubstanceGroups", &getMolSubstanceGroups,
                "returns a copy of the molecule's SubstanceGroups (if any)",
                python::with_custodian_and_ward_postcall<0, 1>());
    python::def("GetMolSubstanceGroupWithIdx", &getMolSubstanceGroupWithIdx,
                "returns a particular SubstanceGroup from the molecule",
                python::return_internal_reference<
                    1, python::with_custodian_and_ward_postcall<0, 1>>());
    python::def("ClearMolSubstanceGroups", &clearMolSubstanceGroups,
                "removes all SubstanceGroups from a molecule (if any)");
    python::def("CreateMolSubstanceGroup", &createMolSubstanceGroup,
                (python::arg("mol"), python::arg("type")),
                "creates a new SubstanceGroup associated with a molecule",
                python::return_value_policy<
                    python::reference_existing_object,
                    python::with_custodian_and_ward_postcall<0, 1>>());
  }
};
}  // namespace RDKit

void wrap_sgroup() { RDKit::sgroup_wrap::wrap(); }
