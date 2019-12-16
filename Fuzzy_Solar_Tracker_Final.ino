//https://github.com/zerokol/eFLL
//Embedded Fuzzy Logic library
#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>
#include <Arduino.h>

#define MOTOR_STEPS 200
#define DIR 8
#define STEP 9
#define ENABLE 11
#define SLEEP 10


//https://github.com/laurb9/StepperDriver
//Stepper Motor Driver DRV8825 library
#include "DRV8825.h"
#define MODE0 5
#define MODE1 6
#define MODE2 7
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MODE0, MODE1, MODE2);

//int RPM;
int photocellPin = 0;
int photocellPin1 = 1;
int LDR_E;
int LDR_W;
int LDR_W_I;
int lastLDR_E = LDR_E;
int lastLDR_W = LDR_W;
float a = 0.0048875855;
float Error;
float LDR_E_V;
float LDR_W_V;
int abs_output;
unsigned long timeNow;
unsigned long timeLast;
unsigned long check_again;

Fuzzy* fuzzy = new Fuzzy();

FuzzySet* NL = new FuzzySet(-5, -5, -5, -3.5);
FuzzySet* NM = new FuzzySet(-5, -3.5, -3.5, -1.5);
FuzzySet* NS = new FuzzySet(-3.5, -1.5, -1.5, 0);
FuzzySet* ZE = new FuzzySet(-1.5, 0, 0, 1.5);
FuzzySet* PS = new FuzzySet(0, 1.5, 1.5, 3.5);
FuzzySet* PM = new FuzzySet(1.5, 3.5, 3.5, 5);
FuzzySet* PL = new FuzzySet(3.5, 5, 5, 5);

FuzzySet* NLo = new FuzzySet(-120, -80, -80, -79);
FuzzySet* NMo = new FuzzySet(-80, -40, -40, -39);
FuzzySet* NSo = new FuzzySet(-40, -20, -20, 0);
FuzzySet* ZEo = new FuzzySet(-20, 0, 0, 20);
FuzzySet* PSo = new FuzzySet(0, 20, 20, 40);
FuzzySet* PMo = new FuzzySet(39, 40, 40, 80);
FuzzySet* PLo = new FuzzySet(79, 80, 80, 120);

void setup() {
  Serial.begin(9600);
  digitalWrite(SLEEP, HIGH);
  stepper.enable();

  FuzzyInput* error = new FuzzyInput(1);
  error->addFuzzySet(NL);
  error->addFuzzySet(NM);
  error->addFuzzySet(NS);
  error->addFuzzySet(ZE);
  error->addFuzzySet(PS);
  error->addFuzzySet(PM);
  error->addFuzzySet(PL);

  fuzzy->addFuzzyInput(error);

  FuzzyOutput* speed = new FuzzyOutput(1);
  speed->addFuzzySet(NLo);
  speed->addFuzzySet(NMo);
  speed->addFuzzySet(NSo);
  speed->addFuzzySet(ZEo);
  speed->addFuzzySet(PSo);
  speed->addFuzzySet(PMo);
  speed->addFuzzySet(PLo);

  fuzzy->addFuzzyOutput(speed);

  FuzzyRuleAntecedent* ifErrornl = new FuzzyRuleAntecedent();
  ifErrornl->joinSingle(NL);
  FuzzyRuleAntecedent* ifErrornm = new FuzzyRuleAntecedent();
  ifErrornm->joinSingle(NM);
  FuzzyRuleAntecedent* ifErrorns = new FuzzyRuleAntecedent();
  ifErrorns->joinSingle(NS);
  FuzzyRuleAntecedent* ifErrorze = new FuzzyRuleAntecedent();
  ifErrorze->joinSingle(ZE);
  FuzzyRuleAntecedent* ifErrorps = new FuzzyRuleAntecedent();
  ifErrorps->joinSingle(PS);
  FuzzyRuleAntecedent* ifErrorpm = new FuzzyRuleAntecedent();
  ifErrorpm->joinSingle(PM);
  FuzzyRuleAntecedent* ifErrorpl = new FuzzyRuleAntecedent();
  ifErrorpl->joinSingle(PL);


  FuzzyRuleConsequent* thenSpeed1nlo =  new FuzzyRuleConsequent();
  thenSpeed1nlo->addOutput(NLo);
  FuzzyRuleConsequent* thenSpeed1nmo =  new FuzzyRuleConsequent();
  thenSpeed1nmo->addOutput(NMo);
  FuzzyRuleConsequent* thenSpeed1nso =  new FuzzyRuleConsequent();
  thenSpeed1nso->addOutput(NSo);
  FuzzyRuleConsequent* thenSpeed1zeo =  new FuzzyRuleConsequent();
  thenSpeed1zeo->addOutput(ZEo);
  FuzzyRuleConsequent* thenSpeed1pso =  new FuzzyRuleConsequent();
  thenSpeed1pso->addOutput(PSo);
  FuzzyRuleConsequent* thenSpeed1pmo =  new FuzzyRuleConsequent();
  thenSpeed1pmo->addOutput(PMo);
  FuzzyRuleConsequent* thenSpeed1plo =  new FuzzyRuleConsequent();
  thenSpeed1plo->addOutput(PLo);


  FuzzyRule* fuzzyRule1 = new FuzzyRule(1, ifErrornl, thenSpeed1nlo);
  fuzzy->addFuzzyRule(fuzzyRule1);
  FuzzyRule* fuzzyRule2 = new FuzzyRule(2, ifErrornm, thenSpeed1nmo);
  fuzzy->addFuzzyRule(fuzzyRule2);
  FuzzyRule* fuzzyRule3 = new FuzzyRule(3, ifErrorns, thenSpeed1nso);
  fuzzy->addFuzzyRule(fuzzyRule3);
  FuzzyRule* fuzzyRule4 = new FuzzyRule(4, ifErrorze, thenSpeed1zeo);
  fuzzy->addFuzzyRule(fuzzyRule4);
  FuzzyRule* fuzzyRule5 = new FuzzyRule(5, ifErrorps, thenSpeed1pso);
  fuzzy->addFuzzyRule(fuzzyRule5);
  FuzzyRule* fuzzyRule6 = new FuzzyRule(6, ifErrorpm, thenSpeed1pmo);
  fuzzy->addFuzzyRule(fuzzyRule6);
  FuzzyRule* fuzzyRule7 = new FuzzyRule(7, ifErrorpl, thenSpeed1plo);
  fuzzy->addFuzzyRule(fuzzyRule7);

}

void motorMove1() {
  stepper.setMicrostep(8);
  stepper.setRPM(abs_output);
  stepper.startMove(1 * 1);
}

void motorMove2() {
  stepper.setMicrostep(8);
  stepper.setRPM(abs_output);
  stepper.startMove(1 * -1);
}

void loop() {
  timeNow = micros();
  checkReading (LDR_E, LDR_W);
  if (LDR_E < lastLDR_E) {
    if (timeNow - check_again >= 1000000) {
      check_again = timeNow;
      checkReading (LDR_E, LDR_W);
    }
    else {
      return;
    }
  }
  errorCheck();
  fuzzyOutput();
  if (LDR_E < lastLDR_E && LDR_W < lastLDR_W) {
    if (timeNow - timeLast >= 120000000) {
      timeLast = timeNow;
      stepper.setMicrostep (8);
      stepper.setRPM(1);
      stepper.move (1 * -1);
    }
    return;

  }
  else if (Error <= -a * 3) {
    motorMove1();
  }

  else if (Error >= a * 3) {
    motorMove2();
  }

  lastLDR_E = LDR_E;
  lastLDR_W = LDR_W;
  timeLast = timeNow;
  stepper.nextAction();
}

void checkReading (int &LDR_E, int &LDR_W) {
  LDR_W_I = analogRead(photocellPin);
  LDR_E = analogRead(photocellPin1);
  LDR_W = LDR_W_I - 2;
}

void errorCheck() {
  LDR_E_V = LDR_E * a;
  LDR_W_V = LDR_W * a;
  Error = (LDR_E_V - LDR_W_V);
}

void fuzzyOutput () {
  fuzzy->setInput(1, Error);
  fuzzy->fuzzify();
  int output = fuzzy->defuzzify(1);
  abs_output = abs(output);
}
