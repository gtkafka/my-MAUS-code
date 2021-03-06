/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "json/json.h"

#include "Geant4/globals.hh"
#include "Geant4/G4StepLimiter.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4ProcessTable.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4PhysListFactory.hh"

#include "Geant4/G4Cerenkov.hh"
#include "Geant4/G4Scintillation.hh"
#include "Geant4/G4OpAbsorption.hh"
#include "Geant4/G4OpRayleigh.hh"
#include "Geant4/G4OpBoundaryProcess.hh"

#include "Interface/Squeak.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"

namespace MAUS {

const std::string MAUSPhysicsList::_scatNames[] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd",
                  "ElectroNuclear", "msc"};
const std::string MAUSPhysicsList::_eLossNames[] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd", "muIoni",
                  "hIoni",  "eIoni"};
const int         MAUSPhysicsList::_nScatNames   = 8;
const int         MAUSPhysicsList::_nELossNames  = 9;

MAUSPhysicsList::MAUSPhysicsList(G4VModularPhysicsList* physList)
                                                       : G4VUserPhysicsList() {
  _list = physList;
}

MAUSPhysicsList::~MAUSPhysicsList()
{}

MAUSPhysicsList* MAUSPhysicsList::GetMAUSPhysicsList() {
  Json::Value& dc = *Globals::GetInstance()->GetConfigurationCards();
  std::string physModel = JsonWrapper::GetProperty
                     (dc, "physics_model", JsonWrapper::stringValue).asString();
  G4PhysListFactory fact;
  if (fact.IsReferencePhysList(physModel)) {
    MAUSPhysicsList* mpl =
                     new MAUSPhysicsList(fact.GetReferencePhysList(physModel));
    try {
        mpl->Setup();
        return mpl;
    }
    catch(Squeal squee) {
        delete mpl;
        throw squee;
    }
  } else {
    throw(Squeal(Squeal::recoverable,
        "Failed to recognise physics list model "+physModel,
        "MAUSPhysicsList::GetMAUSPhysicsList()")
    );
  }
}

void MAUSPhysicsList::BeginOfReferenceParticleAction() {
  Squeak::mout(Squeak::debug)
        << "MAUSPhysicsList::BeginOfReferenceParticleAction() with de model "
        << _refDEModel << std::endl;
  SetStochastics(no_scat, _refDEModel, no_hadronic, false);
}

void MAUSPhysicsList:: BeginOfRunAction() {
  Squeak::mout(Squeak::debug) << "MAUSPhysicsList::BeginOfRunAction() with"
                              << "\n  de model " << _dEModel
                              << "\n  msc model " << _msModel
                              << "\n  hadronic model " << _hadronicModel
                              << "\n  particle decay " << _partDecay
                              << "\n  pi 1/2 life " << _piHalfLife
                              << "\n  mu 1/2 life " << _muHalfLife << std::endl;
  SetStochastics(_msModel, _dEModel, _hadronicModel, _partDecay);
  SetHalfLife(_piHalfLife, _muHalfLife);
}

void MAUSPhysicsList::SetStochastics(scat scatteringModel,
                      eloss energyLossModel, hadronic hadronicModel,
                      bool decayModel) {
  SetEnergyLoss(energyLossModel);
  SetScattering(scatteringModel);
  SetHadronic(hadronicModel);
  SetDecay(decayModel);
}

void MAUSPhysicsList::SetDecay(bool decay) {
  if (!decay) UIApplyCommand("/process/inactivate Decay");
  else        UIApplyCommand("/process/activate   Decay");
}


void MAUSPhysicsList::SetEnergyLoss(eloss eLossModel) {
  double       cutDouble = _productionThreshold;
  std::stringstream cutStream;
  std::string  elossActive = "activate";
  std::string  flucActive  = "true";
  switch (eLossModel) {
    case energyStraggling:
      elossActive = "activate";
      flucActive  = "true";
      cutDouble   =  _productionThreshold;
      break;
    case dedx:
      elossActive = "activate";
      flucActive  = "false";
      cutDouble   = 1e12*mm;
      break;
    case no_eloss:
      elossActive = "inactivate";
      flucActive  = "false";
      cutDouble   = 1e12*mm;
      break;
  }
  cutStream << cutDouble;
  std::vector<std::string> uiCommand;
  uiCommand.push_back("/run/setCut "+cutStream.str());
  uiCommand.push_back("/process/eLoss/fluct "+flucActive);
  for (int i = 0; i < _nELossNames; i++)
    uiCommand.push_back("/process/"+elossActive+" "+_eLossNames[i]);

  for (size_t i = 0; i < uiCommand.size(); i++) {
    Squeak::mout(Squeak::debug) << "Applying " << uiCommand[i] << std::endl;
    UIApplyCommand(uiCommand[i]);
  }
}


void MAUSPhysicsList::SetScattering(scat scatteringModel) {
  std::string activation;
  switch (scatteringModel) {
    case mcs:
      activation  = "/process/activate ";
      break;
    case no_scat:
      activation  = "/process/inactivate ";
      break;
  }

  for (int i = 0; i < _nScatNames; i++) {
    Squeak::mout(Squeak::debug) << "Applying " << activation+_scatNames[i]
                                << std::endl;
    UIApplyCommand(activation+_scatNames[i]);
  }
}

void MAUSPhysicsList::SetHadronic(hadronic hadronicModel) {
  std::string activation;
  switch (hadronicModel) {
    case all_hadronic:
      activation  = "/process/activate ";
      break;
    case no_hadronic:
      activation  = "/process/inactivate ";
      break;
  }

  G4ProcessVector*   pvec  = G4ProcessTable::GetProcessTable()->
                                                       FindProcesses(fHadronic);
  for (int i = 0; i < pvec->size(); i++) {
    std::string pname = (*pvec)[i]->GetProcessName();
    Squeak::mout(Squeak::debug) << "Applying " << activation+pname << std::endl;
    UIApplyCommand(activation+pname);
  }
}

void MAUSPhysicsList::ConstructParticle() {
    _list->ConstructParticle();
}

void MAUSPhysicsList::ConstructProcess() {
    _list->ConstructProcess();
    SetSpecialProcesses();
    std::cerr << "CONSTRUCTPROCESS" << std::endl;
    ConstructOp();
}

void MAUSPhysicsList::ConstructOp()
{
    G4Cerenkov* theCerenkovProcess = new G4Cerenkov("Cerenkov");
    G4Scintillation* theScintillationProcess = new G4Scintillation("Scintillation");
    G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
    G4OpRayleigh* theRayleighScatteringProcess = new G4OpRayleigh();
    G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();
    theCerenkovProcess->SetMaxNumPhotonsPerStep(20);
    theScintillationProcess->SetScintillationYieldFactor(1.);
    theScintillationProcess->SetTrackSecondariesFirst(true);

    G4OpticalSurfaceModel themodel = unified;
    theBoundaryProcess->SetModel(themodel);

    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
      if (theCerenkovProcess->IsApplicable(*particle)) {
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
      if (theScintillationProcess->IsApplicable(*particle)) {
        pmanager->AddProcess(theScintillationProcess);
        pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
        pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
      }
      if (particleName == "opticalphoton") {
        G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
        pmanager->AddDiscreteProcess(theAbsorptionProcess);
        pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
        pmanager->AddDiscreteProcess(theBoundaryProcess);
    }
  }
  std::cerr << "CONSTRUCT OP" << std::endl;
}

void MAUSPhysicsList::SetSpecialProcesses() {
  theParticleIterator->reset();  // from G4VUserPhysicsList
  while ( (*theParticleIterator)() ) {
    G4ProcessManager* pmanager = theParticleIterator->value()->
                                                            GetProcessManager();
    pmanager->AddProcess(new G4StepLimiter, -1, -1, 2);
  }
}

void MAUSPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife) {
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MAUSPhysicsList::SetParticleHalfLife(std::string particleName,
                                          double halfLife) {
  if (halfLife <= 0.) return;
  std::stringstream ss_in;
  ss_in << halfLife;
  UIApplyCommand("/particle/select "+particleName);
  UIApplyCommand("/particle/property/lifetime "+ss_in.str()+" ns");
  UIApplyCommand("/particle/property/dump");
}

void MAUSPhysicsList::Setup() {
    Json::Value& dc = *Globals::GetInstance()->GetConfigurationCards();
    std::string refPhysicsModel = JsonWrapper::GetProperty(dc,
            "reference_physics_processes", JsonWrapper::stringValue).asString();
    std::string physicsModel = JsonWrapper::GetProperty(dc, "physics_processes",
                                          JsonWrapper::stringValue).asString();

    if (refPhysicsModel == "none")
      _refDEModel = no_eloss;
    if (refPhysicsModel == "mean_energy_loss")
      _refDEModel = dedx;

    if (physicsModel == "none") {
      _msModel = no_scat;
      _dEModel = no_eloss;
      _hadronicModel = no_hadronic;
    }

    if (physicsModel == "mean_energy_loss") {
      _msModel = no_scat;
      _dEModel = dedx;
      _hadronicModel = no_hadronic;
    }

    if (physicsModel == "standard") {
      _msModel = mcs;
      _dEModel = energyStraggling;
      _hadronicModel = all_hadronic;
    }

    _partDecay = JsonWrapper::GetProperty(dc, "particle_decay",
                                          JsonWrapper::booleanValue).asBool();
    _piHalfLife = JsonWrapper::GetProperty(dc, "charged_pion_half_life",
                                          JsonWrapper::realValue).asDouble();
    _muHalfLife = JsonWrapper::GetProperty(dc, "muon_half_life",
                                          JsonWrapper::realValue).asDouble();
    _productionThreshold = JsonWrapper::GetProperty(dc, "production_threshold",
                                          JsonWrapper::realValue).asDouble();
}

void MAUSPhysicsList::UIApplyCommand(std::string command) {
    Squeak::mout(Squeak::debug)
        << "Apply G4UI command: " << command << std::endl;
    G4UImanager::GetUIpointer()->ApplyCommand(command);
}
}

