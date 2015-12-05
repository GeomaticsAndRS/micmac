/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/

#include "Vino.h"

#if (ELISE_X11)

/*******************************************************************/
/*                                                                 */
/*    cPopUpMenuMessage                                            */
/*                                                                 */
/*******************************************************************/

void CorrectNonEmpty(int &aV0,int & aV1, const int & aVMax)
{
    if (aV0>aV1) ElSwap(aV0,aV1);

    if (aV0==aV1)
    {
         if (aV1<aVMax) 
            aV1++;
         else 
            aV0--;
    }
}

void CorrectRect(Pt2di &  aP0,Pt2di &  aP1,const Pt2di & aSz)
{
    aP0 = Inf(aSz,Sup(aP0,Pt2di(0,0)));
    aP1 = Inf(aSz,Sup(aP1,Pt2di(0,0)));

    CorrectNonEmpty(aP0.x,aP1.x,aSz.x);
    CorrectNonEmpty(aP0.y,aP1.y,aSz.y);
}


/*******************************************************************/
/*                                                                 */
/*    cPopUpMenuMessage                                            */
/*                                                                 */
/*******************************************************************/


cPopUpMenuMessage::cPopUpMenuMessage(Video_Win aW,Pt2di aSz) :
   PopUpMenuTransp(aW,aSz)
{
}

void cPopUpMenuMessage::ShowMessage(const std::string & aName, Pt2di aP,Pt3di aCoul)
{
     UpP0(aP);
     Pt2di aLarg = mW.SizeFixedString(aName);
     mW.fixed_string
     (
           Pt2dr(aP+ (mSz+Pt2di(-aLarg.x, aLarg.y))/2)  ,
           aName.c_str(), mW.prgb()(aCoul.x,aCoul.y,aCoul.z),
           true
     );
}

void cPopUpMenuMessage::Hide()
{
    Pop();
}

/*******************************************************************/
/*                                                                 */
/*    cStatImageRehauss                                            */
/*                                                                 */
/*******************************************************************/


void FillStat(cXml_StatVino & aStat,Flux_Pts aFlux,Fonc_Num aFonc)
{
   aFonc = Rconv(aFonc);
   int aNbCh = aFonc.dimf_out();
   aStat.Soms().resize(aNbCh,0.0);
   aStat.Soms2().resize(aNbCh,0.0);
   aStat.ECT().resize(aNbCh,0.0);
   aStat.VMax().resize(aNbCh,0.0);
   aStat.VMin().resize(aNbCh,0.0);
   Symb_FNum aSF(aFonc);

   ELISE_COPY
   (
        aFlux,
        Virgule(1.0,aSF,Square(aSF)),
        Virgule
        (
            sigma(aStat.Nb()),
            sigma(VData(aStat.Soms()),aNbCh)  | VMin(VData(aStat.VMin()),aNbCh) | VMax(VData(aStat.VMax()),aNbCh),
            sigma(VData(aStat.Soms2()),aNbCh)
        )
   );

   double aNb = aStat.Nb();
   aStat.IntervDyn() = Pt2dr(0,0);

   for (int aK=0 ; aK<aNbCh ; aK++)
   {
       aStat.Soms()[aK] /= aNb;
       aStat.Soms2()[aK] /= aNb;
       aStat.ECT()[aK] = sqrt(ElMax(0.0,aStat.Soms2()[aK]-ElSquare(aStat.Soms()[aK])));
       aStat.IntervDyn().x += aStat.VMin()[aK];
       aStat.IntervDyn().y += aStat.VMax()[aK];
   }
    aStat.IntervDyn() = aStat.IntervDyn() / aNbCh;
}

/****************************************/
/*                                      */
/*          STRING                      */
/*                                      */
/****************************************/

std::string StrNbChifSignNotSimple(double aVal,int aNbCh)
{
   if (aVal==1) return "1";
   if (aVal==0) return "0";
   if (aVal==-1) return "-1";

   std::string aStrSign = "";
   if (aVal<0)
   {
         aVal = - aVal;
         aStrSign = "-";
   }

   if (aVal < 1)
   {
        if (aVal>0.1) return  ToString(aVal).substr(0,aNbCh+2);
        if (aVal>0.01) return  ToString(aVal).substr(0,aNbCh+3);

        double aLog10 = log(aVal) / log(10);
        int aLogDown =  round_down(ElAbs(aLog10));
        aVal = ElMin(1.0,aVal * pow(10,aLogDown));

        return aStrSign  + ToString(aVal).substr(0,aNbCh+2) + "E-" + ToString(aLogDown);


   }

   if (aVal<100)
   {
       std::string aRes = ToString(aVal).substr(0,aNbCh+1);
       return aStrSign  + aRes;
   }

   double aLog10 = log(aVal) / log(10);
   int aLogDown =  round_down(ElAbs(aLog10));
   aVal = ElMin(10.0,aVal / pow(10,aLogDown));
   return aStrSign  + ToString(aVal).substr(0,aNbCh+2) +  "E" + ToString(aLogDown);
}

std::string StrNbChifSign(double aVal,int aNbCh)
{
    return SimplString(StrNbChifSignNotSimple(aVal,aNbCh));
}

std::string SimplString(std::string aStr)
{
   if (aStr.find('.') == std::string::npos)
      return aStr;
   int aK= aStr.size()-1;
   while ((aK>0) && (aStr[aK]=='0'))
     aK--;
   if (aStr[aK]=='.')
     aK--;
   aK++;
   return aStr.substr(0,aK);
}


std::string StrNbChifApresVirg(double aVal,int aNbCh)
{
    std::string aRes = ToString(aVal);
    int aKp = aRes.find('.');

    if (int(std::string::npos) == aKp)
    {
       return aRes;
    }

    int aK;
    for (aK = aKp ; (aK< aKp+aNbCh+1) && (aRes[aK]!=0) ; aK++) ;


    return SimplString(aRes.substr(0,aK));
}


/*
std::string StrNbChifSign(double aVal,int aNbCh)
{
    return SimplString(StrNbChifSignNotSimple(aVal,aNbCh));
}
*/

/****************************************/
/*                                      */
/*            Icone                     */
/*                                      */
/****************************************/

Im2D_U_INT1 Icone(const std::string & aName,const Pt2di & aSzCase,bool Floutage,bool Negatif)
{
   cElBitmFont & aFont = cElBitmFont::BasicFont_10x8() ;

   Im2D_Bits<1> aImBin = aFont.MultiLineImageString(aName,Pt2di(0,5),-aSzCase,0);
   ELISE_COPY(aImBin.border(3),1,aImBin.out());
   ELISE_COPY(aImBin.border(1),0,aImBin.out());

   Pt2di  aSz = aImBin.sz();
   Im2D_U_INT1 aRes(aSz.x,aSz.y);

   if (Negatif)
   {
      ELISE_COPY(line(Pt2di(0,0),aSz),1,aImBin.oclip());
      ELISE_COPY(line(Pt2di(aSz.x,0),Pt2di(0,aSz.y)),1,aImBin.oclip());
      ELISE_COPY(aImBin.all_pts(),!aImBin.in(),aImBin.out());
   }

   ELISE_COPY
   (
      aRes.all_pts(),
      (!aImBin.in(0)) *255,
      aRes.out()
   );


   if (Floutage)
   {
       ELISE_COPY
       (
            aRes.all_pts(),
            (  Negatif                                   ?
               Max(aRes.in(),rect_som(aRes.in(0),1)/9.0) :
               Min(aRes.in(),rect_som(aRes.in(0),1)/9.0)
            ),
            aRes.out()
       );

   }

   return aRes;
}

void PutFileText(Video_Win aW,const std::string & aName)
{
   ELISE_fp  aFile (aName.c_str(),ELISE_fp::READ);

   int aCpt=0;
   
   char * aCstr = 0;
   while ((aCstr = aFile.std_fgets()))
   {
       aW.fixed_string(Pt2dr(10,15*aCpt +20),aCstr,aW.pdisc()(P8COL::black),true);
       aCpt++;
   }
   aFile.close();
}


bool TreeMatchSpecif(const std::string & aNameFile,const std::string & aNameSpecif,const std::string & aNameObj)
{
    cElXMLTree aFullTreeParam(aNameFile);
    cElXMLTree * aTreeParam = aFullTreeParam.GetUnique(aNameObj,false);
    cElXMLTree aTreeSpec(StdGetFileXMLSpec(aNameSpecif));
    return  aTreeParam->TopVerifMatch(&aTreeSpec,aNameObj,true);
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/


     // ============== cCaseX11Xml =======================================

class cCaseX11Xml
{
    public :
       void Efface();
       void Efface(int aCoul);
       static cCaseX11Xml * Alloc(Video_Win aW,Box2di aBox,int aCoul);
       void string(int aPos,const std::string & );
       bool Inside(const Pt2di &) const;
       static int  GetCase(const std::vector<cCaseX11Xml *> &,const Pt2di &);
    private :
       cCaseX11Xml(Video_Win aW,Box2di aBox,int aCoul);
       Video_Win mW;
       Box2di    mBox;
       int       mCoul;
};

void cCaseX11Xml::string(int aPos,const std::string & aName)
{
   mW.fixed_string_middle(mBox,aPos,aName,mW.pdisc()(P8COL::black),true);
}


void cCaseX11Xml::Efface(int aCoul)
{
   if (aCoul >=0)
      mW.fill_rect(Pt2dr(mBox._p0),Pt2dr(mBox._p1),mW.pdisc()(aCoul));
}
void cCaseX11Xml::Efface() {Efface(mCoul);}

cCaseX11Xml * cCaseX11Xml::Alloc(Video_Win aW,Box2di aBox,int aCoul)
{
   return new cCaseX11Xml(aW,aBox,aCoul);
}

cCaseX11Xml::cCaseX11Xml(Video_Win aW,Box2di aBox,int aCoul) :
    mW    (aW),
    mBox  (aBox),
    mCoul (aCoul)
{
    Efface();
}
bool cCaseX11Xml::Inside(const Pt2di & aP) const { return mBox.inside(aP); }


int  cCaseX11Xml::GetCase(const std::vector<cCaseX11Xml *> & aVC,const Pt2di & aP)
{
     for (int  aKC=0 ; aKC<int (aVC.size()) ; aKC++)
     {
          if (aVC[aKC]->Inside(aP))
             return aKC;
     }
     return -1;
}



     // ============== cWindowXmlEditor =======================================

class cWindowXmlEditor
{
     public :
         cWindowXmlEditor(Video_Win aW);
         Box2di  TopDraw(cElXMLTree * aTree);
         void Interact();
     private :
         int  EndXOfLevel(int aLevel);
         Box2di  PrintTag(Pt2di aP0,cElXMLTree *,int aMode,int aLev) ; // 0 => terminal, 1 ouvrant , 2 fermant
         Box2di  Draw(Pt2di ,cElXMLTree * aTree ,int aLev);

         Video_Win    mW;
         // std::vector<cElXMLTree *> mTrees;

         Pt2di                     mPRab;
         int                       mDecalX;
         int                       mSpace;
         std::vector<cCaseX11Xml*> mVCase;
         cCaseX11Xml *             mCaseQuit;
         
};


cWindowXmlEditor::cWindowXmlEditor
(
     Video_Win aW
) :
    mW  (aW),
    mPRab  (Pt2di(12,8)),
    mDecalX   (30),
    mSpace    (5),
    mCaseQuit (0)
{
}

void cWindowXmlEditor::Interact()
{
    while (1)
    {
        Clik aClik = mW.clik_in();
        int aKC =  cCaseX11Xml::GetCase(mVCase,round_ni(aClik._pt));
        if (aKC>=0)
        {
           cCaseX11Xml * aCase = mVCase[aKC];
           if (aCase == mCaseQuit)
           {
              return;
           }

           std::cout << "KC = " << aKC << "\n";
        }
    }
}


Box2di  cWindowXmlEditor::PrintTag(Pt2di aP0,cElXMLTree * aTree,int aMode,int aLevel) 
{
    std::string aTag =  ((aMode == -1) ? "</" : "<") + aTree->ValTag() + ((aMode==0) ? "/>" : ">");
    Pt2di aSz = mW.SizeFixedString(aTag);
    mW.fixed_string(Pt2dr(aP0)+Pt2dr(0,aSz.y), aTag.c_str(),mW.pdisc()(P8COL::black),true);

    Box2di aRes  (aP0-mPRab,aP0+aSz+ mPRab);
    if (aMode ==0)
    {
         Pt2di aQ0 (aP0.x+aSz.x+5,aP0.y-2);
         Pt2di aQ1 (EndXOfLevel(aLevel)-5,aP0.y+aSz.y+2);
         mVCase.push_back(cCaseX11Xml::Alloc(mW,Box2di(aQ0,aQ1),P8COL::yellow));
    }

    return aRes;
}

int  cWindowXmlEditor::EndXOfLevel(int aLevel)
{
   return mW.sz().x-((aLevel+1)*mDecalX)/2;
}


Box2di cWindowXmlEditor::Draw(Pt2di aP0,cElXMLTree * aTree,int aLev)
{
     if (aTree->Profondeur() <= 1)
     {
          return PrintTag(aP0,aTree,0,aLev);
     }

     aP0.y +=mSpace;

     Box2di aRes = PrintTag(aP0,aTree,1,aLev);

      for
      (
            std::list<cElXMLTree *>::iterator itF= aTree->Fils().begin();
            itF != aTree->Fils().end();
            itF++
      )
      {
            
           Box2di aBox = Draw(Pt2di(aP0.x+mDecalX,aRes._p1.y),*itF,aLev+1);
           aRes = Sup(aRes,aBox);
      }

      
     Box2di aBoxFerm = PrintTag(Pt2di(aP0.x,aRes._p1.y),aTree,-1,aLev);

     aRes =  Sup(aBoxFerm,aRes);
     mW.draw_rect(Pt2dr(aRes._p0),Pt2dr(EndXOfLevel(aLev),aRes._p1.y),mW.pdisc()(P8COL::red));
     aRes._p1.y += mSpace; 
     return aRes;
}

Box2di cWindowXmlEditor::TopDraw(cElXMLTree * aTree)
{
    ELISE_COPY(mW.all_pts(),196,mW.ogray());

    int aXMil = mW.sz().x/2;

    if (mCaseQuit==0)
    {
       mCaseQuit = cCaseX11Xml::Alloc(mW,Box2di(Pt2di(aXMil-100,10),Pt2di(aXMil+100,40)),P8COL::magenta);
    }
    mCaseQuit->Efface();
    mCaseQuit->string(0,"Quit edit");
    
    mVCase.clear();
    mVCase.push_back(mCaseQuit);

    return Draw(Pt2di(50,50),aTree,0);
}


void TestXmlX11()
{
    Video_Win aW =  Video_Win::WStd(Pt2di(700,800),1.0);
     
    cElXMLTree aFullTreeParam("toto.xml");
    aFullTreeParam.StdShow("tata.xml");
    cWindowXmlEditor aWX(aW);
  
    aWX.TopDraw(aFullTreeParam.Fils().front());

    aWX.Interact();

}


#endif





/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
