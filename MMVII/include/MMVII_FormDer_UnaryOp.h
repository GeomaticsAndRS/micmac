#ifndef _MMVII_FormDer_UnaryOp_H_
#define _MMVII_FormDer_UnaryOp_H_

/** \file MMVII_FormDer_UnaryOp.h
    \brief File for unary operator on formal derivative
*/


namespace  NS_MMVII_FormalDerivative
{

/* *************************************************** */
/* *************************************************** */
/* *                                                 * */
/* *        UNARY FORMULA                            * */
/* *                                                 * */
/* *************************************************** */
/* *************************************************** */


            /*  ----------------------------------------------------------
               Class implementing unary operation on formula
                  MOTHER CLASS : cUnaryF
                  DERIVED :   cSquareF / cExpF / cMin1F / cLogF
             ----------------------------------------------------------------*/

template <class TypeElem> class cUnaryF : public cImplemF<TypeElem>
{
      public :
            typedef cImplemF<TypeElem> tImplemF;
            typedef typename tImplemF::tCoordF tCoordF;
            typedef typename tImplemF::tFormula tFormula;
            typedef typename tImplemF::tBuf     tBuf;

            virtual std::string  NameOperator() const = 0;
            virtual std::string  PostName() const {return "";}
            std::string  InfixPPrint() const override 
            {
               return NameOperator() + " "+  mF->InfixPPrint() + PostName() ;
            }

            /// In the cas an additional parameter is used, as "powc F30 3.14"
            TypeElem Extrac1Param (const std::string & aString)
            {
                std::string aBuf1,aBuf2;
                TypeElem aVal;

                std::stringstream aStream(aString);

                aStream >> aBuf1 >> aBuf2 >> aVal;
                return aVal;
            }

      protected  :
            virtual std::string genCodeNAddr() const override
            {
                return NameOperator() + "(" + mF->genCodeFormName() + ")";
            }

            virtual std::string genCodeDef() const override
            {
                return NameOperator() + "(" + mF->genCodeRef() + ")";
            }

            std::vector<tFormula> Ref() const override{return std::vector<tFormula>{mF};}
            inline cUnaryF(tFormula aF,const std::string & aName) :
                 tImplemF (aF->CoordF(),aName),
                 mF       (aF),
                 mDataF   (mF->DataBuf())
            { }
            tFormula        mF;    ///< The formula argument of unary operator
            const TypeElem* mDataF; ///< Fast access to data of buf
};


/**  Classes for square */
template <class TypeElem> class cSquareF : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;
     
            cSquareF (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName)
            { }
      private :
            std::string  NameOperator() const override {return "square";}
            std::string genCodeNAddr() const override {
                return  mF->genCodeFormName()  + " * " + mF->genCodeFormName();
            }
            std::string genCodeDef() const override {
                return "(" + mF->genCodeRef() + " * " + mF->genCodeRef() + ")";
            }
            void ComputeBuf(int aK0,int aK1) override
            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] =  mDataF[aK] * mDataF[aK];
            }
            ///  rule : (FF)' =   2 F' F
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return  2.0  * mF->Derivate(aK)  * mF;
            }
};

template <class TypeElem> class cCubeF : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;
     
            cCubeF (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName)
            { }
      private :
            std::string  NameOperator() const override {return "cube";}
            virtual std::string genCodeNAddr() const override {
                  return mF->genCodeFormName() + " * " + mF->genCodeFormName() + " * " + mF->genCodeFormName();
            }

            virtual std::string genCodeDef() const override {
                return "(" + mF->genCodeRef() + " * " + mF->genCodeRef() + " * " + mF->genCodeRef() + ")";
            }

            void ComputeBuf(int aK0,int aK1) override

            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] =  mDataF[aK] * mDataF[aK] * mDataF[aK];
            }
            ///  rule : (F^3)' =   3 F' F^2
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return  3.0  * mF->Derivate(aK)  * square(mF);
            }
};


template <class TypeElem> class cExpF : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;

            cExpF (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName)
            { }
      private :
            std::string  NameOperator() const override {return "exp";}
            void ComputeBuf(int aK0,int aK1) override  
            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] = std::exp(mDataF[aK]);
            }
            ///  rule : (exp FF)' =   F' exp F
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return   mF->Derivate(aK)  * exp(mF);
            }
};

template <class TypeElem> class cMin1F : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;

            cMin1F (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName)
            { }
      private :
            std::string  NameOperator() const override {return "-";}
            void ComputeBuf(int aK0,int aK1) override  
            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] = - mDataF[aK];
            }
            /// rule : (-F)'  =  - (F')
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return   - mF->Derivate(aK)  ;
            }
};

template <class TypeElem> class cLogF : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;

            cLogF (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName)
            { }
      private :
            std::string  NameOperator() const override {return "log";}
            void ComputeBuf(int aK0,int aK1) override  
            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] = std::log(mDataF[aK]);
            }
            /// rule : (log F)'  =  F' / F
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return   mF->Derivate(aK) / mF ;
            }
};


template <class TypeElem> class cPowCste : public cUnaryF<TypeElem>
{
     public :
            using cUnaryF<TypeElem>::mF;
            using cUnaryF<TypeElem>::mDataF;
            using cImplemF<TypeElem>::mDataBuf;

            cPowCste (cFormula<TypeElem> aF,const std::string & aName) :
                cUnaryF <TypeElem> (aF,aName),
                mExp  (cUnaryF<TypeElem>::Extrac1Param (aName))
            { 
            }
      private :
            std::string  NameOperator() const override {return "powc";}
            virtual std::string  PostName() const {return " " + std::to_string(mExp);}

            virtual std::string genCodeNAddr() const override {
                return "pow(" + mF->genCodeFormName() + "," + std::to_string(mExp) + ")";  // FIXME: verifier precision
            }

            virtual std::string genCodeDef() const override {
                return "pow(" + mF->genCodeRef() + "," + std::to_string(mExp) + ")";  // FIXME: verifier precision
            }

            void ComputeBuf(int aK0,int aK1) override  
            {
                for (int aK=aK0 ; aK<aK1 ; aK++)
                    mDataBuf[aK] = std::pow(mDataF[aK],mExp);
            }
            /// rule : (log F)'  =  F' / F
            cFormula<TypeElem> Derivate(int aK) const override 
            {
                return   (mExp*mF->Derivate(aK)) * pow(mF,mExp-1.0);
            }

            TypeElem mExp;
};


      /* ---------------------------------------*/
      /*           Global Functio on unary op   */
      /* ---------------------------------------*/

/**  A Helper class to avoid code duplication on the process :
       * compute name
       * test existence
       * eventualy create
     Template parameter is the class created
*/

template <class TypeCompiled>  class cGenOperatorUnaire
{
    public :
         typedef typename TypeCompiled::tCoordF     tCoordF;
         typedef typename TypeCompiled::tImplemF     tImplemF;
         typedef typename tImplemF::tFormula  tFormula;

         static tFormula   Generate(tFormula aF,const std::string & aNameOp,const std::string & Aux="")
         {
             tCoordF* aPCont = aF->CoordF();  // Get the context from the formula
             std::string aNameForm = aF.NameFormulaUn(aNameOp,Aux);  // Compute the name formula should have

             if (aPCont->ExistFunc(aNameForm))  // If it already exist 
               return aPCont->FuncOfName(aNameForm);  // Then return formula whih this name

             tFormula aResult (new TypeCompiled(aF,aNameForm)); // else create it
             aPCont->AddFormula(aResult); // indicate to the context to remember this new formula 
             return aResult;              // return it
         }
};
    
template <class TypeElem> 
inline cFormula<TypeElem>  square(const cFormula<TypeElem> & aF)
{
    return cGenOperatorUnaire<cSquareF<TypeElem> >::Generate(aF,"square");
}

template <class TypeElem> 
inline cFormula<TypeElem>  cube(const cFormula<TypeElem> & aF)
{
    return cGenOperatorUnaire<cCubeF<TypeElem> >::Generate(aF,"cube");
}

template <class TypeElem> 
inline cFormula<TypeElem> exp(const cFormula<TypeElem> & aF)
{
    return cGenOperatorUnaire<cExpF<TypeElem> >::Generate(aF,"exp");
}
template <class TypeElem> 
inline cFormula<TypeElem>  operator - (const cFormula<TypeElem> & aF)
{
    const TypeElem * aC = aF->ValCste();
    if (aC)
        return aF->CoordF()->CsteOfVal(- *aC);  // help reduce other operation on '-0' too

    return cGenOperatorUnaire<cMin1F<TypeElem> >::Generate(aF,"-");
}
template <class TypeElem> 
inline cFormula<TypeElem>  log (const cFormula<TypeElem> & aF)
{
    return cGenOperatorUnaire<cLogF<TypeElem> >::Generate(aF,"log");
}

template <class TypeElem> 
inline cFormula<TypeElem>  pow (const cFormula<TypeElem> & aF,const TypeElem& aVal )
{
    if (aVal==TypeElem(2)) return square(aF);
    if (aVal==TypeElem(3)) return cube(aF);
    return cGenOperatorUnaire<cPowCste<TypeElem> >::Generate(aF,"powc",std::to_string(aVal));
}

} //   NS_MMVII_FormalDerivative



#endif // _MMVII_FormDer_UnaryOp_H_
