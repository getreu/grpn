/*

  Copyright (C) 2002  Paul Wilkins

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/* funcs.c  by Paul Wilkins */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <ctype.h>

#include "stack.h"
#include "number.h"
#include "funcs.h"
#include "undo.h"
#include "constant.h"
#include "editor.h"
#include "run_cmd.h"
#include "error.h"
#include "mode.h"

void decomposeMtrx();
void createMtrx();
void decomposeCmplx();
void createCmplx();
Number * readNumber(char *line, int *nread);
int isDigitBase(char c);

void readLine(char *line) {
    int pos = 0;
    Number *n1;
    struct lconv *locale;
   
    locale = localeconv();

    if (*line != '"' &&
	*line != '\'' &&
	(isDigitBase(*line) ||
	 *line == '-' ||
	 *line == '+' ||
	 *line == *(locale->decimal_point))) {

	/* try to read in a number */
	n1 = readNumber(line, &pos);
	if (n1 == NULL) {
	    setStringError("Error: Cannot read number.");
	    return;
	}

	SaveStackState(0);  /* what to restore to the stack after an undo */
	Push(n1);
	UndoStackState(1);  /* what to undo from the stack after an undo */

	/* what's left must be a command */
    }

    runCmd(line+pos);
}



void DupStack() {
    Number *n1;

    finishEditor();

    if (checkArgs("Dup", 1))
	return;
    SaveStackState(0);  /* what to restore to the stack after an undo */

    n1 = getStackEle(0);
    Push(setNumberNumber(newNumber(), n1));

    UndoStackState(1);  /* what to undo from the stack after an undo */
}

void PopStack() {
    if (checkArgs("Drop", 1))
	return;
    SaveStackState(1);

    freeNumber(Pop());

    UndoStackState(0);
}

void PushStack() {
    if (isEditingEditor())
	finishEditor();
    else
	DupStack();
}

void IStack() {
    Number *n1;

    finishEditor();

    SaveStackState(0);

    n1 = setNumberCmplx(newNumber(), cmplxI);
    Push(n1);

    UndoStackState(1);
}

void EStack() {
    Number *n1;

    finishEditor();

    SaveStackState(0);

    n1 = setNumberReal(newNumber(), realE);
    Push(n1);

    UndoStackState(1);
}

void PiStack() {
    Number *n1;

    finishEditor();

    SaveStackState(0);

    n1 = setNumberReal(newNumber(), realPi);
    Push(n1);

    UndoStackState(1);
}

void NegStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("+/-", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = negNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void InvStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("Inv", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = invNumber(n1);
    if(n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}


void LnStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("ln", 1))
	return;
    SaveStackState(1);

    n1=Pop();
    n2 = lnNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void LogStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("log", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = logNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void TenxStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if(checkArgs("10^x", 1))
	return;
    SaveStackState(1);

    n3 = setNumberReal(newNumber(), realTen);

    n1 = Pop();
    n2 = powNumber(n3, n1);
    freeNumber(n3);

    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void ExpStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("exp", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = expNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void PowStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("pow", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = powNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void NrootStack() {
    Number *n1, *n2, *n3, *n4;

    finishEditor();

    if (checkArgs("nroot", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n4 = invNumber(n1);
    if (n4 == NULL) {
	Push(n1);
	CancelStackState();
	return;
    }

    n2 = Pop();
    n3 = powNumber(n2, n4);
    freeNumber(n4);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void SqrStack() {
    Real *r1;
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("sqr", 1))
	return;
    SaveStackState(1);

    r1 = setRealDouble(newReal(), 2.0);
    n3 = setNumberReal(newNumber(), r1);
    freeReal(r1);

    n1 = Pop();
    n2 = powNumber(n1, n3);
    freeNumber(n3);

    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void SqrtStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("sqrt", 1))
	return;
    SaveStackState(1);

    n3 = setNumberReal(newNumber(), realHalf);

    n1 = Pop();
    n2 = powNumber(n1, n3);
    freeNumber(n3);

    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void Db10Stack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("db10", 1))
	return;
    SaveStackState(1);

    n1 = Pop();

    n2 = dbNumber(n1, 10.0);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void Db20Stack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("db20", 1))
	return;
    SaveStackState(1);

    n1 = Pop();

    n2 = dbNumber(n1, 20.0);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void RipStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("rip", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = ripNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void SinStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("sin", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = sinNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void CosStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("cos", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = cosNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void TanStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("tan", 1))
	return;
    SaveStackState(1);

    n1=Pop();
    n2 = tanNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void AsinStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("asin", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = asinNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void AcosStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("acos", 1))
	return;
    SaveStackState(1);

    n1=Pop();
    n2 = acosNumber(n1);
    if (n2) {
	Push(n2);
	freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void AtanStack() {
    Number *n1, *n2;

    finishEditor();

    if(checkArgs("atan", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    n2 = atanNumber(n1);
    if (n2) {
	Push(n2); freeNumber(n1);
	UndoStackState(1);
    } else {
	Push(n1);
	CancelStackState();
    }
}

void AddStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("+", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = addNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void SubStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("-", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = subNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void MulStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("*", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = mulNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void DivStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("/", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = divNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void ModStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("mod", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = modNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void SwapStack() {
    Number *n1, *n2;

    finishEditor();

    if (checkArgs("swap", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    Push(n1);
    Push(n2);

    UndoStackState(2);
}

void CplxStack() {
    Number *n1;

    finishEditor();

    if (checkArgs("cplx", 1))
	return;
    n1 = getStackEle(0);

    switch (n1->type) {
    case COMPLEX:
	decomposeCmplx();
	break;
    case REAL:
	createCmplx();
	break;
    default:
	setStringError("complex Error: Bad Argument Type.");
    }
}

void createCmplx() {
    Number *n1, *n2;
    Cmplx *c1;

    if (checkArgs("cplx", 2))
	return;
    SaveStackState(2);
  
    n1 = Pop();
    n2 = Pop();
    if (n1->type != REAL || n2->type != REAL) {
	setStringError("-> complex Error: Bad Argument Type.");
	Push(n2);
	Push(n1);
	CancelStackState();
	return;
    }
    c1 = inputCmplxReal(newCmplx(), n2->data, n1->data);
    freeNumber(n1);
    freeNumber(n2);
    Push(setNumberCmplx(newNumber(), c1));
    freeCmplx(c1);

    UndoStackState(1);
}

void decomposeCmplx () {
    Real *re1, *re2;
    Number *n1;
    Cmplx *a, *c1;

    if (checkArgs("cplx", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    a = (Cmplx *)n1->data;
    re1 = newReal();
    re2 = newReal();

    if (getPolarMode() == POLAR) {
	c1 = polarCmplx(a);
	setRealReal(re1, c1->re);
	if(getRadixMode() == DEGREES){
	    mulEqReal(c1->im, real180Pi);
	    setRealReal(re2, c1->im);
	} else {
	    setRealReal(re2, c1->im);
	}
	freeCmplx(c1);
    } else {
	setRealReal(re1, a->re);
	setRealReal(re2, a->im);
    }

    Push(setNumberReal(newNumber(), re1));
    Push(setNumberReal(newNumber(), re2));
    freeReal(re1);
    freeReal(re2);
    freeNumber(n1);
    UndoStackState(2);
}

/* either make a matrix or decompose a matrix */
void MtrxStack(){
    Number *n1;

    finishEditor();

    if (checkArgs("matrix", 1))
	return;

    n1 = getStackEle(0);

    switch(n1->type){
    case COMPLEX:
	setStringError("matrix Error: Bad Argument Type.");
	break;
    case MATRIX:
	decomposeMtrx();
	break;
    case REAL:
	createMtrx();
	break;
    }
}


void decomposeMtrx() {
    int i, j, matSize;
    Number *n1, **ptr;
    Real *rs, *cs;
    Matrix *m1;

    if (checkArgs("matrix", 1))
	return;
    SaveStackState(1);

    n1 = Pop();
    m1 = (Matrix *)n1->data;
    matSize = m1->rows * m1->cols;

    for (i = 0; i < m1->rows; i++) {
	for(j = 0; j < m1->cols; j++) {
	    ptr = m1->data + (m1->cols * i) + j;
	    Push(setNumberNumber(newNumber(), *ptr));
	}
    }
    Push(setNumberReal(newNumber(), setRealDouble((cs=newReal()), m1->cols)));
    Push(setNumberReal(newNumber(), setRealDouble((rs=newReal()), m1->rows)));
    freeReal(cs);
    freeReal(rs);

    freeNumber(n1);

    UndoStackState(2 + matSize);
}


void createMtrx() {
    int i, j;
    int rows, cols;
    Number *n1, *n2;
    Matrix *m1;

    if (checkArgs("matrix", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    if (n1->type != REAL || n2->type != REAL) {
	setStringError("matrix Error: Bad Argument Type.");
	Push(n2);
	Push(n1);
	CancelStackState();
	return;
    }
    rows = (int)setDoubleReal(n1->data);
    cols = (int)setDoubleReal(n2->data);
    if (rows == 0 || cols == 0) {
	setStringError("matrix Error: Bad Argument Type.");
	Push(n2);
	Push(n1);
	CancelStackState();
	return;
    }

    if (checkArgs("matrix", rows * cols)) {
	Push(n2);
	Push(n1);
	CancelStackState();
	return;
    }
    freeNumber(n1);
    freeNumber(n2);

    AddStackState(rows * cols);

    /* read in the matrix */
    m1 = newMatrix();

    for (i = rows - 1; i >= 0; i--){
	for(j = cols - 1; j >= 0; j--){
	    n1 = Pop();
	    setMatrix(m1, n1, i, j);

	    freeNumber(n1);
	}
    }

    Push(setNumberMatrix(newNumber(), m1));

    freeMatrix(m1);

    UndoStackState(1);
}

void LShiftStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("lshift", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = lShiftNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void RShiftStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("rshift", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = rShiftNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void BitwiseANDStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("and", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = bitwiseANDNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void BitwiseORStack() {
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("or", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = bitwiseORNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

void BitwiseXORStack(){
    Number *n1, *n2, *n3;

    finishEditor();

    if (checkArgs("xor", 2))
	return;
    SaveStackState(2);

    n1 = Pop();
    n2 = Pop();
    n3 = bitwiseXORNumber(n2, n1);
    if (n3) {
	Push(n3);
	freeNumber(n1);
	freeNumber(n2);
	UndoStackState(1);
    } else {
	Push(n2);
	Push(n1);
	CancelStackState();
    }
}

#define IS_HEX(c) isxdigit((c))
#define IS_DEC(c) isdigit((c))
#define IS_OCT(c) ((c)=='0' || (c)=='1' ||	\
		   (c)=='2' || (c)=='3' ||	\
		   (c)=='4' || (c)=='5' ||	\
		   (c)=='6' || (c)=='7')
#define IS_BIN(c) ((c)=='0' || (c)=='1') 

int isDigitBase(char c){
    switch (getBaseMode()) {
    case HEXADECIMAL:
	return IS_HEX(c);
	break;
    case DECIMAL_ENG:
	return IS_DEC(c);
	break;
    case DECIMAL:
	return IS_DEC(c);
	break;
    case OCTAL:
	return IS_OCT(c);
	break;
    case BINARY:
	return IS_BIN(c);
	break;
    }
    fprintf(stderr, "Invalid Base Mode: %#x\n", getBaseMode());
    return 0;
}


Number * readNumber(char *line, int *nread) {
    char *p, buf[2];
    int i1;
    double num;
    Real *r1;
    Number *n1;

    /* read in the number */
    switch (getBaseMode()) {
    case HEXADECIMAL:
	*nread = 0;
	/* look for "0x" */
	if (0 == strncmp(line, "0x", 2)) {
            line += 2;
            *nread = 2;
	}
	num = 0.0;
	buf[1] = '\0';
	for (p = line; IS_HEX(*p); p++, (*nread)++) {
            buf[0] = *p;
            sscanf(buf, "%x", &i1);
            num *= 16.0;
            num += (double)i1;
	}
	if (p-line == 0)
	    return NULL;
	break;

    case DECIMAL:
    case DECIMAL_ENG:
	/* the line starts with a number-- read it in */
	if (1 != sscanf(line, "%lg%n", &num, nread)) {
	    return NULL;
	}
	break;

    case OCTAL:
	*nread = 0;
	num = 0.0;
	buf[1] = '\0';
	for (p = line; IS_OCT(*p); p++, (*nread)++) {
            buf[0] = *p;
            sscanf(buf, "%d", &i1);
            num *= 8.0;
            num += (double)i1;
	}
	if (p-line == 0)
	    return NULL;
	break;
    case BINARY:
	*nread = 0;
	num = 0.0;
	for (p = line; IS_BIN(*p); p++, (*nread)++) {
            num *= 2.0;
            if (*p == '1')
		num += 1.0;
	}
	if (p-line == 0)
	    return NULL;
	break;
    }

    r1 = setRealDouble(newReal(), num);
    n1 = setNumberReal(newNumber(), r1);
    freeReal(r1);

    return n1;
}
