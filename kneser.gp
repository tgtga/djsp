/* https://tetrationforum.org/showthread.php?tid=486 */

\p 67;       /* default precision 32 decimal digits accuracy, if you want faster results, use \p 28 */
/* for higher precision, use values of \p 133 or \p 105 */

/* L, B, Period idelta are global variables set by init(base) */
renormup  =     1; /* used in the recenterup routine, for experiments, try renormup=0, =2 */
curprecision=0;
lastprecision=-2;
idelta    = 12*I/100;  /* default idelta */
debugloop =     0;     /* debug loop plot control */
B         =     0;     /* sexp(z) base, initialized by init(base) */
etaB      = exp(1/exp(1));
L         = 0.318 + 1.337*I; /* set by init(b) */
imult     =     1; /* also set by init(b), corresponds to e^2pi*idelta */
recenter  =     0; /* updated by recenterup called by init(b) */
centerat  =     0; /* optional centerat value for sexp, for bases>20  */
renorm    =     1; /* set by init(b) */
gennewsexp =    0; /* use the riemB function for bases<eta */
lastradius = 0.65; /* used by loop(t), use 0.65 for last iteration, and don't update precision */
loopradius = 0.50; /* used by loop(t), use 0.50 for other iterations, experiment */
strmmult   =  3/2;
loopcount =     0; /* loopcount */
noboundchk =    0;
initxsuperf=    0;
quietmode =     0; /* default quitemode is off; don't print anything when initbase */

/* dummy global variables initializations, actually set by init and loop */
goalbits  =     0; /* calculated by init(B) */
precis    =     0; /* calculated in init(B) */
Period    =     1; /* period calculated by init */
xterminc  =     0; /* init stabililzation parameter for B<1.7 */
sfuncprec = 1E-32; /* set by rawinit, used by initsuperf; used by slog */
scnt      =     0; /* set by initsuperf; for Schroder/iSchroder initialization */
scnt2     =     0; /* set by initsuperf; for B<etaB superf2/isuperf2 routines */

/* stuff added for cheta */
chterms   = 51;
chdelta   = 100;
invchetr  = 0.006;
usematrix = 1;
chetadlt  = 0;
sxpetadlt = 0;
xtheta    = 0;
xsexp     = 0;

sieinit() = {
  if (B<3,
    xsexp = 1 + x; /* sexp(0)=1, renorminit will fill in x, x^2 terms */
  , /* else B>=3 */
    xsexp  = (rlnB+lnlnB)/2 + x*(rlnB-lnlnB); /* linear approximation, renorminit will fill in x, x^2 terms */
  );
  recenter = 0;
  loopcount=0;
  recenterup();   /* recenter sexp(z) algorithm so that sexp(-1)=0, sexp(0)=1 */
}

rawinit(initbase) = {
  local(s,y,z,lastabs, curabs, lastl, lastprecis);
  if (initbase==0, initbase=exp(1)); B=initbase;
  /* approximation for base B, first derivative continuous */
  lnB = log(B);
  rlnB = 1/lnB;
  lnlnB = log(rlnB)*rlnB;
  L=0.5;
  curabs = 1;
  lastabs = 1;
  s=120; if (B<1.7, s=300); if (B<1.47, s=1800);
  while ((curabs<lastabs) || (s>0),
    lastabs = curabs;
    lastl=L;
    L=log(L)*rlnB;
    curabs = abs(lastl-L);
    s--;
  );
  /* Period = 2*Pi*I/(L*log(B)+log(log(B))); */
  Period = 2*Pi*I/log(log(L));
  if (B<etaB,
    L2=0.5;
    curabs = 1;
    lastabs = 1;
    s=300;
    while ((curabs<lastabs) || (s>0),
      lastabs = curabs;
      lastl=L2;
      L2=exp(L2*lnB);
      curabs = abs(lastl-L2);
      s--;
    );
    /* Period2 = -2*Pi*I/(L2*log(B)+log(log(B))); */
    Period2 = 2*Pi*I/log(log(L2));
  );
  /* precision goal bits for loop(t) */
  z=1.0;
  lastprecis=precis; /* used to detect initialization required for cheta */
  precis=precision(z);
  sfuncprec=10^-((precis/2)-1.5); /* used by initsuperf; used by slog */
  y=1.0;while(z<>(z+y),y=y/2);
  goalbits = abs(log(sqrt(y)*50)/log(2))-10;  /* subtract 10 for the last iteration */
  loglogL = log(L*lnB);
  rloglogL = 1/loglogL;
/*logL = L*lnB;*/
  rlogL = 1/(L*lnB);
  imult = exp(-idelta*I*2*Pi);
  xterminc = 0; /* xterminc used to stabilize isuperf for bases<1.7 */
  if ((B<1.7),  xterminc=-2);  /* limited convergence range for isuperf(z), superf/thetaaprx are not effected */
  centerat=0; if ((B>20),   centerat=-0.5);
  if ((B>5000), centerat=-0.65);
  strmmult=3/2;
  curprecision=0;
  sieinit();
  if (quietmode==0,
    default(format, "g0.24");
    print ("   base          " B);
    print ("   fixed point   " L);
    print ("   Pseudo Period " Period);
  );
  initxsuperf=0;
  /* init base cheta stuff, for initial precision<>0 or current precision<>67 */
  if (precis<>lastprecis, initcheta());
  return(1);
}

init(initbase) = {
  rawinit(initbase);
  loop();
  if (quietmode==0,
    if (precis>67, default(format, "g0.64"),
      if (precis==67, default(format, "g0.32"), default(format,"g0.14"));
    );
  );
  return(1);
}

superf2(z) = {
/* complex valued superfunction for base B, generated from the fixed point L2 */
/* only valid for B<eta */
  local (y);
  if (B>etaB, print ("superf2 only valid for B<eta"),
    y=z+zerosf2+scnt2;
    y=L2-(L2*lnB)^y; /* logL=L*log(B) */
    for (i=1,scnt2, y=log(y)*rlnB);
    return(y);
  );
}

isuperf2(z) = {
/* complex valued inverse superfunction for base B, generated from the fixed point L */
  local (y,sc,logL2);
  y=z;
  for (i=1,scnt2, y=exp(y*lnB)); /* rlnB=1/log(B) */
  y=y-L2;
  logL2 = L2*lnB;
  sc=(logL2)^(-scnt2-zerosf2);   /* logL=log(L*lnB) */
  y=-y*sc;
  y=log(y)/log(logL2);
  return(y);
}

superf(z) = {
/* complex valued superfunction for base B, generated from the fixed point L */
  local (x0,y,i,sc);
  x0=exp(z*loglogL);
  sc=0;
  while (abs(x0)>rsuper,x0=x0*rlogL;sc++);
  y=subst(iSchroder,x,x0);
  for (i=1,sc, y=exp(y*lnB));
  return(y);
}

isuperf(z) = {
/* complex valued inverse superfunction for base B, generated from the fixed point L */
  local (x0,y,sc);
  x0=z;
  sc=0;
  while (abs(x0-L)>rsuper, x0=log(x0)*rlnB; sc++);
  x0=x0-L;
  y=subst(Schroder,x,x0);
  y = y*exp(loglogL*sc); /* loglogL = log(L*lnB); */
  y=log(y)*rloglogL; /* rloglogL = 1/log(L*lnB); */
  return(y);
}

initsuperf(samples) = {
  local(i,rinv,s,t,y,z,tot,t_est,tcrc,halfsamples,ym,xc,superm,isuperm);
/* Schroder/iSchroder polynomial series initialization, used for superf/isuperf */
/* scnt is the number iterations required series initialization */
  y=0.5;
  s=0;
  while ((abs(y-L)>sfuncprec), y=log(y)*rlnB; s++; );
  scnt=s;
  if (B<etaB, init_lt_etaB()); /* scnt2, and zerosf2 are used by the superf2/isuperf2 routines for base<etaB */
  if (quietmode==0, print ("generating inverse Schroder taylor series for superf function, scnt "scnt));
  initxsuperf=1;
  halfsamples=samples/2;
  t_est    = vector (samples,i,0);
  tcrc     = vector (samples,i,0);
  rsuper=100;
  xc=0;
  i=0; /* this is for bases<etaB */
  while ((xc<8) && (i<50),
    i++;
    ym=abs(xc-L);
    if (ym<rsuper,rsuper=ym);
    xc=B^xc;
  );
  rsuper=0.47*rsuper; /* 0.47^99=3.45E-33, length(xsuper)=floor(1.5*precis)=100 */
  rinv = 1/rsuper;
  for(s=1, samples, xc=-1/(samples)+(s/halfsamples); tcrc[s]=exp(Pi*I*xc); );

  superm = exp(-scnt*loglogL); /* multiplier for superfunction */
  isuperm = exp(scnt*loglogL); /* multiplier for inverse superfunction */
  for (t=1,samples,
    y = L+rsuper*tcrc[t]*superm;
    for (i=1, scnt, y=exp(y*lnB);y=precision(y,precis) ); /* y=precision(y,precis) is the patch for init(exp(Pi/2));loop; */
    t_est[t] = y;
  );

  iSchroder=0;
  for (s=0,floor(samples*200/240)-1,
    tot=0;
    for (t=1,samples,
      tot=tot+t_est[t];
      t_est[t]=t_est[t]*conj(tcrc[t]);
    );
    tot=(tot/samples)*(rinv)^s;
    tot=precision(tot,precis);
    iSchroder=iSchroder+tot*x^s;
  );

  if (quietmode==0, print ("generating Schroder taylor series for isuperf function, scnt "scnt));
  for (t=1,samples,
    y = L+rsuper*tcrc[t];
    for (i=1,scnt, y=log(y)*rlnB); /* rlnB=1/log(B) */
    y=y-L;
    /* isuperm = exp(scnt*loglogL); */
    y=y*isuperm;
    t_est[t] = y;
  );

  Schroder=0;
  for (s=0,floor(samples*200/240)-1,
    tot=0;
    for (t=1,samples,
      tot=tot+t_est[t];
      t_est[t]=t_est[t]*conj(tcrc[t]);
    );
    tot=(tot/samples)*(rinv)^s;
    tot=precision(tot,precis);
    Schroder=Schroder+tot*x^s;
  );

  /* zerosf1 for historical reference only */
  if (B<etaB, zerosf1=real(isuperf(superf2(Period2/2)))+Period/2; );

}

init_lt_etaB() = {
  local(s,t,y,lastt);
  /* for bases<etaB, zerosf2 used by superf2/isuperf2 */
  /* for bases<etaB, scnt2 is the number iterations required for superf2/isuperf2 routines */
  y=0.5;
  s=0;
  while ((abs(y-L2)>sfuncprec), y=exp(y*lnB); s++; );
  scnt2=s;

  zerosf2=1;
  s=1;
  t=1;
  lastt=1;
  while ((s<10) || abs(t)<abs(lastt),
    lastt=t;
    t = -(superf2(0)-1)/0.6;
    zerosf2 = zerosf2+t;
    s++;
  );
}

sexp(z) = {
/* sexp approximation from unit circle Taylor series.  Converges nicely for -1<imag(z)<1.  Real(z) over a very large range   */
  local (i,s,t);
  if (abs(imag(z))>0.87, /* sqrt(3)/2 */
    /* use thetaaprx(z) if imag(z) is too big, or too small */
    if (imag(z)>0, return(thetaaprx(z)), return(conj(thetaaprx(conj(z)))));
  );
  z=z+recenter-centerat;
  t = floor(real(z)+0.5);
  z = z-t;
  s = subst(xsexp,x,z);
  while ((t<0), s=log(s)*rlnB; t++; );
  while ((t>0), s=exp(s*lnB);  t--; );
  return(s);
}

thetaaprx(w) = {
  local(w1,y,z,x0,a);
/*if (imag(w)<imag(idelta-0.002*I), print ("!!! WARNING, sexp(z) much better than thetaaprx(z) for imag(z)<0.12*I !!!")); */
  if ((B>etaB) || gennewsexp,
    x0 = exp(w*2*Pi*I);
    y = subst(xtheta,x,x0)+w;
    z = superf(y);
    return(z);
  );
  if ((B<etaB) && (gennewsexp==0),
    /* generates the sexp from the attracting fixed point via */
    /* kneser mapping from the repelling fixed point */
    /* Period2  = Period - 2*I*imag(rie[1]); */
    /* if imag(w)=Period2/2, imag(riemaprxB(w))=0 */
    /* f(Period2/2 + w) = conj(f(conj(Period2/2 - w))) */
    /*                                      */
    a  = polcoeff(xtheta,0);
    crossover = Period - 2*I*imag(a); /* crossover = Period2; */
    w1 = conj(w)+crossover;
    x0 = exp(w*2*Pi*I);
    y  = subst(xtheta,x,x0)+w;
    x0 = exp(w1*2*Pi*I);
    y  = y + conj(subst(xtheta,x,x0)) - conj(a);
    z  = superf(y);
    return(z);
  );
}

thetaup(rtrm) = {
  local(s,t,x1,m,tot,ltot,precis2,rer,rcrc);

  if (rtrm==0, rtrm=length(xtheta));
  rer       = vector (rtrm*2);
  rcrc      = vector (rtrm*2);
  for (s=1,rtrm*2,
    x1 = -1 - 1/(rtrm*4) + (s/(rtrm*2)) + xterminc; /* riemann full circle, from -1 to 0, -pi to +pi, +pi to -pi, in slow mode, avoid the singularity in the horizontal direction */
    rcrc[s]=exp(-2*Pi*I*x1);
    rer[s] = isuperf(precision(sexp(x1+idelta),precis))-x1;    /* make sure we have full precision before 100's of isuperf log iterations */
  );
  m=1;
  xtheta=0;
  for (t=0,rtrm-1,
    tot=0;
    for (s=1,2*rtrm,
      tot=tot+rer[s];
      rer[s]=rer[s]*rcrc[s];
    );
    tot=tot/(2*rtrm);
    if ((t>0), m=m*imult; tot=tot*m); /* convert rie back to idelta=0 format */
    if ((t>=8) && (noboundchk==0),
      if (( abs(tot) > abs(ltot) ), t=rtrm-1;tot=0); /* stability, chaos prevention */
    );
    tot=precision(tot,precis); /* required precision update for xtheta as a polynomial */
    xtheta=xtheta+tot*x^t;
    ltot=tot;
  );
  /* convert rie back to idelta=0 format, allows easy comparisons between rie arrays for different runs */
  xtheta=xtheta-idelta;
}

testidelta(ysexp) = {
  local(y);
  y = subst(ysexp,x,0.5+idelta);
  y = -y + exp(lnB*subst(ysexp,x,-0.5+idelta));
  return(y);
}

renormsub() = {
  local (renorm0,renorm0t,delt,renorm0s,renorm1s,x0,x1,x0s,x1s,denom,s,x2sexp);
  renorm0 = testidelta(xsexp);
  renorm0t = renorm0;
  for (s=1,2,
    if ((renorm0t==0), renorm0t=testidelta(xsexp));
    delt = abs(renorm0t);
    if (delt<>0,

      x2sexp = xsexp + delt;
      renorm0s = testidelta(x2sexp);
      x0s=(renorm0t-renorm0s)/delt; /* slope of x0 with respect to renorm */

      x2sexp = xsexp + x*delt;
      renorm1s = testidelta(x2sexp);
      x1s=(renorm0t-renorm1s)/delt; /* slope of x with respect to renorm */

      /*   solve for a1, a2, which are the delta values for sie[1], sie[2], respectively */
      /*   a1*imag(x0s)+a2*imag(x1s) = imag(renorm0);  */
      /*   a1*real(x0s)+a2*real(x1s) = real(renorm0);  */
      denom = real(x1s)*imag(x0s) - imag(x1s)*real(x0s);
      if (denom<>0,
        x0 = (imag(renorm0t)*real(x1s) - real(renorm0t)*imag(x1s)) / denom;
        x1 = (real(renorm0t)*imag(x0s) - imag(renorm0t)*real(x0s)) / denom;
        xsexp = xsexp + x0 + x1*x;
      );
      renorm0t=0;
    );
  );
  return(renorm0);
}

renorminit() = {
  local (renorm0,delt,renorm2s,renorm1s,x2,x1,x2s,x1s,denom,s,mys,x2sexp);
/* allows smooth bases at idelta, essential for initializing bases near eta, without large values of recenter */
/* for larger bases, provides a smooth transition initialization at idelta, preserving sie[1] */
/* this allows convergence for bases up to 2 million */

  renorms = testidelta(xsexp);
  renorm0 = renorms;
  delt = 5;
/*sie[1]=1;*/
  mys=10^(-precis+5);
  s=1;
  while ((delt>mys) && (s<100),
    x2sexp = xsexp;
    if ((renorm0==0), renorm0 = testidelta(xsexp));
    delt = abs(renorm0);
    if (delt<>0,

      x2sexp = xsexp + delt*x^2;
      renorm2s = testidelta(x2sexp);
      x2s=(renorm0-renorm2s)/delt; /* slope of x^2 with respect to renorm */

      x2sexp = xsexp + delt*x;
      renorm1s = testidelta(x2sexp);
      x1s=(renorm0-renorm1s)/delt; /* slope of x with respect to renorm */

      /*   solve for a1, a2, which are the delta values for x^2, x, respectively */
      /*   a1*imag(x2s)+a2*imag(x1s) = imag(renorm0);  */
      /*   a1*real(x2s)+a2*real(x1s) = real(renorm0);  */
      denom = real(x1s)*imag(x2s) - imag(x1s)*real(x2s);
      if (denom<>0,
        x2 = (imag(renorm0)*real(x1s) - real(renorm0)*imag(x1s)) / denom;
        x1 = (real(renorm0)*imag(x2s) - imag(renorm0)*real(x2s)) / denom;
        xsexp = xsexp + x1*x + x2*x^2;
      );
    );
    s++;
    renorm0=0;
  );
  return(renorms);
}

/* renormup=2 for alternative renorminit routine */
recenterup() = {
  local (x1,y);
  if ((abs(recenter)>0.5) && loopcount, recenter=0);  /* after initialization, set recenter=0 */
  /* renorm update */
  if (renormup,
    if ((renormup==2) && (loopcount>=1) && (centerat==0),
      y = polcoeff(xsexp,0);
      xsexp = xsexp-y+1;
    );
    if ((renormup==1) && (loopcount>=1), renorm=abs(renormsub()), renorm=abs(renorminit());)
  );
  /* recenter update */
  if ((B>20) && (loopcount==0), x1=slog(1,0.25), x1=slog(1,0));
  recenter=precision(recenter+real(x1),precis);
}

sexpup(st) = {
  local(s,x0,t,z,tot,ideltai,ltot,scrc,rieest,strm,strmat);
/* use the rie array, and regenerate the "sie" approximation */
  if (st==0, st=ceil(length(xsexp)*(1/strmmult)));
  rieest    = vector (st);
  scrc      = vector (st);
  ideltai  = imag(idelta);
  for (t=1,st,
    x0=-1/(st*2)+(t/st);
    scrc[t]=exp(Pi*I*x0);
    x0=scrc[t];
    if ((imag(x0)>=ideltai),
      rieest[t]=thetaaprx(scrc[t]+centerat),
      rieest[t]=precision(sexp(scrc[t]+centerat),precis);  /* make sure sexp has full precision before 100's of isuperf log iterations, thetaaprx naturally has precision */
    );
  );
  strm = floor(st*strmmult);
  strmat=strm;
  x2sexp = 0;
  for (s=0,strm-1,
    tot=0;
    for (t=1,st,
      tot=tot+real(rieest[t]);
      rieest[t]=rieest[t]*conj(scrc[t]);
    );
    tot=tot/st;
    x2sexp = x2sexp + tot*x^s;

    if (noboundchk==0,
      if ((s%2), /* odd Taylor series terms should always be positive */
        if ((tot<0), strmat=s-1; s=strm-1);
      );
      if ((s%2==0) && (s>0), /* even Taylor series, sie[s-2]<0 => sie[s]<0 */
        ltot = polcoeff(x2sexp,s-2);
        if (((tot>0) && (ltot<0)), strmat=s-1; s=strm-1);
      );
    );
  );
  xsexp=0;
  for (s=0,strmat-1,
    xsexp=xsexp+polcoeff(x2sexp,s)*x^s;
  );
}

erroravg() = {
  local(tot,z,x0);
  x0 = sexp(-0.5);
  tot=0;
  kt=37;
  for (s=1,37,
    z=-0.5-1/(2*kt)+(s/kt)+idelta;
    tot=tot+abs(sexp(z)-thetaaprx(z));
  );
  tot=-log(tot/kt)/log(2);
  if (quietmode==0,
    if (precis>67, default(format, "g0.64"),
      if (precis==67, default(format, "g0.32"), default(format,"g0.14"));
    );
    print ("sexp(-0.5)= " x0);
    default(format, "g0.10");
    if (loopcount<10,print1(" "));
    print (loopcount "=loopcnt  " tot " theta/sexp binary precision bits");
    if (precis>67, default(format, "g0.64"), default(format, "g0.32"));
  );
  return(tot);
}

slog(z,est) = {
  local (y,s,slop,lastyz,curyz,lest,ly,jt,e);
/* inverse sexp(z), using "est" as initial estimate */
/* est is an optional parameter, important near the singularity */
/* the 0.01 radius used for the slope means won't converge within 0.01 of singularity */
  e=exp(1);
  jt=0;
  if ((imag(z)==0) && (est==0),
    while ((z>B) || (z>e), z=rlnB*log(z);jt++);
    if (z<0,z=exp(z*lnB);jt=-1);
  );
  lastyz=100;
  y=sexp(est);
  curyz=abs(y-z);
  lest=est+curyz*0.05;
  ly=sexp(lest);
  s=1;
  while ((curyz>sfuncprec) && ((curyz<lastyz) || (s<3)),
    est=precision(est,precis);
    y=precision(y,precis);
    slop=(y-ly)/(est-lest);
    lest=est;
    ly=y;
    est=est+(z-y)/slop;
    lastyz=curyz;
    y=sexp(est);
    curyz=abs(y-z);
    s++;
  );
  if (curyz>0.1, print (z" bad result, need better initial est slog(z,est)"));
  return(est+jt);
}

/* split the imag(z) into three cases, for optimal precision in the complex plane */
/* this function is deprecated, and is now incorporated into sexp(z) */
splicesexp(z) = { if (imag(z)>=0.8, thetaaprx(z), if (imag(z)<=-0.8, conj(thetaaprx(conj(z))), sexp(z))); }

sexptaylor( w,r) = {
  local(rinv,s,t,x0,y,z,tot,t_est,tcrc,wsexp);
/* outputs polynomial taylor tseries for the complex sexp function */
  t_est    = vector (240,i,0);
  tcrc     = vector (240,i,0);
  if (loopcount==0,print("init required before generating taylor series");return(0));
  if (r==0,r=1);
  rinv = 1/r;
  for(s=1, 240, x0=-1/(120*2)+(s/120); tcrc[s]=exp(Pi*I*x0); );

  for (t=1,240, t_est[t] = sexp(w+r*tcrc[t]); );
  wsexp=0;
  for (s=0,199,
    tot=0;
    for (t=1,240,
      tot=tot+t_est[t];
      t_est[t]=t_est[t]*conj(tcrc[t]);
    );
    tot=(tot/240);
    if (s>=1, tot=tot*(rinv)^s);
    if ((imag(w)==0) && (real(w)>-2), tot=real(tot));
    wsexp=wsexp+tot*x^s;
  );
  wsexp=precision(wsexp,precis);
  return(wsexp);
}

prtpoly(wtaylor,t,name) = {
  local(s,z);
  if (t==0,t=60);
  if (name<>0,
    print ("{"name"=");
    z=polcoeff(wtaylor,0);
    if (real(z)<0, print("      "z), print("        " z));
    for (s=1,t-1,
      z=polcoeff(wtaylor,s);
      if (s>9, print1("+x^" s), print1("+x^ " s));
      print1("* ");
      if (imag(z)<>0, print1("("));
      if (real(z)<0, print1(z), print1(" " z));
      if (imag(z)<>0, print(")"), print(););
    );
    print("}");
  ,
    for (s=0,t-1,
      z=polcoeff(wtaylor,s);
      if (s>9, print1("a" s "= "), print1("a" s "=  "));
      if (real(z)<0, print(z), print(" " z));
    );
  );
}

slogtaylor(w,r,est) = {
  local(rinv,s,t,x0,y,z,tot,t_est,tcrc,wtaylor);
/* outputs polynomial taylor series, the complex taylor series for slog */
/* est is an optional initial estimate for slog(w+r) */
  t_est    = vector (240,i,0);
  tcrc     = vector (240,i,0);
  if (loopcount==0,print("init required before generating taylor series");return(0));
  if (r==0,r=1);
  rinv = 1/r;
  for(s=1, 240, x0=-1/(120*2)+(s/120); tcrc[s]=exp(Pi*I*x0); );
  if (est==0, est=slog(w+r));
  for (t=1,240, est=slog(w+r*tcrc[t],est);t_est[t]=est);
  wtaylor=0;
  for (s=0,199,
    tot=0;
    for (t=1,240,
      tot=tot+t_est[t];
      t_est[t]=t_est[t]*conj(tcrc[t]);
    );
    tot=(tot/240);
    if (imag(w)==0, tot=real(tot));
    if (s>=1, tot=tot*(rinv)^s);
    wtaylor=wtaylor+tot*x^s;
  );
  wtaylor=precision(wtaylor,precis);
  return(wtaylor);
}

/* Cheta, sexp_eta stuff added to kneser.gp */
/* inicheta, cheta, sexpeta, genpoly, */
/* basechange because its the complex base change function, too much fun! */

initcheta() = {
/* automatically initialize cheta during program initialization */
  local(z);
  z=1.0;
  precis=precision(z);
  /* initialization for xcheta and xsexpeta required for cheta, sexpeta, invcheta, invsexpeta functions */
  /* initalizes to match precis, 50 digits/67 digits, this routine aims for 75% precision               */
  /* called from init(initbase) when program is loaded, and when init detects the precision has changed */
  chterms   = 2*(floor(precis/2)-8)+1;
  chdelta   = (chterms-1)*2;
  xcheta = genpoly(chterms,chdelta,0);
  invchetr = imag(cheta(-chdelta+(chterms-1)*I/2));
  invprecis = 10000.*(10^-chterms);
  /* invprecis = 100*abs(chetaerr(I+0.5,chdelta)); */
  /* invprecis = 1E-47; */
  chetadlt=cheta(-chdelta);
  xsexpeta = genpoly(chterms,chdelta,1);
  sxpetadlt=sexpeta(chdelta);
  return(0);
}

cheta(w) = {
  local(i,x0,y,e1);
  e1=exp(-1);
  x0=w;
  i=chdelta;
  while (real(x0)>+1/2,x0--;i++);
  while (real(x0)<-1/2,x0++;i--);
  y = subst(xcheta,x,x0);
  if (i>0, for (s=1,i, y=exp(y*e1)));
  if (i<0, for (s=1,-i,y=log(y)*exp(1)));
  return(y);
}

sexpeta(w) = {
  local(i,x0,y,e,e1);
  e=exp(1);
  e1=exp(-1);
  x0=w;
  i=-chdelta;
  while (real(x0)>+1/2,x0--;i++);
  while (real(x0)<-1/2,x0++;i--);
  y = subst(xsexpeta,x,x0);
  if (i>0, for (s=1,i, y=exp(y*e1)));
  if (i<0, for (s=1,-i,y=log(y)*e));
  return(y);
}

genpoly(mterms,mdelta,forsexp) = {
  local(i,ht,xr,yr,initc,e,e1,fl,m25,s25,r25);
  xr  = vector (mterms,i,0);
  yr  = vector (mterms,i,0);
  if (mdelta==0,mdelta=chdelta);
  ht = (mterms-1)/2;
  e=exp(1);
  e1=1/e;
  if (forsexp==0,
    initc=2*e;
    for (s=1,mdelta-ht,initc=log(initc)*e);
    for (i=1,mterms, yr[mterms+1-i]=initc;initc=log(initc)*e);
  );
  if (forsexp,
    initc=1;
    for (s=1,mdelta-ht,initc=exp(initc*e1));
    for (i=1,mterms, yr[i]=initc;initc=exp(initc*e1));
  );
  if (usematrix==1,
    m25=matrix(ht,ht,i,j,i^(2*j));
    s25=matrix(ht,1,i,j,(yr[ht+1+i]+yr[ht+1-i])/2-yr[ht+1]);
    r25=matsolve(m25,s25);
    fl=0;
    for (s=1,ht,fl=fl+r25[s,1]*x^(s*2));
    m25=matrix(ht,ht,i,j,i^(2*j-1));
    s25=matrix(ht,1,i,j,(yr[ht+1+i]-yr[ht+1-i])/2);
    r25=matsolve(m25,s25);
    for (s=1,ht,fl=fl+r25[s,1]*x^(s*2-1));
    fl = fl + yr[ht+1];
  );
  if (usematrix==2,
    m25=matrix(mterms,mterms,i,j,if ((i==1+ht) && (j==1), 1, (i-1-ht)^(j-1)));
    s25=matrix(mterms,1,i,j,yr[i]);
    r25=matsolve(m25,s25);
    fl=0;
    for (s=1,mterms,fl=fl+r25[s,1]*x^(s-1));
  );
  if (usematrix==0,
    for(i=1,mterms,xr[i]=i-1-ht);
    fl = polinterpolate(xr,yr);
  );
  return(fl);
}

invcheta(z,est) = {
  local(i,t,mz,e,lastyz,curyz,lest,ly,y);
  e=exp(1);
  mz=z;
  t=0;
  if (est==0,
    mz=z;
    while ((abs(mz-chetadlt)>invchetr) && (t<1000),
      t++;
      mz=log(mz)*e;
    );
    if ((t==1000), print ("too close to e " z " " mz));
    est = 2/(chetadlt/e-1) - chdelta - 2/(mz/e-1);
  );
/* inverse sexp(z), using "est" as initial estimate */
/* est is an optional parameter, important near the singularity */
/* the 0.05 radius used for the slope means won't converge within 0.05 of singularity */
  lastyz=100;
  y=cheta(est);
  curyz=abs(y-mz);
  lest=est+curyz*0.05;
  ly=cheta(lest);
  i=40;
  while ((curyz>invprecis) && ((curyz<lastyz) || i),
    est=precision(est,precis);
    y=precision(y,precis);
    slop=(y-ly)/(est-lest);
    lest=est;
    ly=y;
    est=est+(mz-y)/slop;
    lastyz=curyz;
    y=cheta(est);
    curyz=abs(y-mz);
    i--;
  );
  return(est+t);
}

invsexpeta(z,est) = {
  local(i,t,mz,e,e1,lastyz,curyz,lest,ly,y);
  e=exp(1);
  e1=1/e;
  mz=z;
  t=0;
  if (est==0,
    mz=z;
    while ((abs(mz-sxpetadlt)>invchetr) && (t<1000),
      t++;
      mz=exp(mz*e1);
    );
    if ((t==1000), print ("too close to e " z " " mz));
    est = chdelta - 2/(1-sxpetadlt*e1) + 2/(1-mz*e1);
  );
/* inverse sexp(z), using "est" as initial estimate */
/* est is an optional parameter, important near the singularity */
/* the 0.05 radius used for the slope means won't converge within 0.05 of singularity */
  lastyz=100;
  y=sexpeta(est);
  curyz=abs(y-mz);
  lest=est+curyz*0.05;
  ly=sexpeta(lest);
  i=40;
  while ((curyz>invprecis) && ((curyz<lastyz) || i),
    est=precision(est,precis);
    y=precision(y,precis);
    slop=(y-ly)/(est-lest);
    lest=est;
    ly=y;
    est=est+(mz-y)/slop;
    lastyz=curyz;
    y=sexpeta(est);
    curyz=abs(y-mz);
    i--;
  );
  return(est-t);
}

/* gfunc/gie/gtaylor/gaprx is a free extra taylor series routine for use by anything you want */
/* gfunc(z) = { cheta(invcheta(z)+0.5); } */
/* gfunc(z) = { basechange(z,3); } */
/* gfunc(z) = { invcheta(z) } */
/* gfunc(z) = { sexpeta(z) } */
/* gfunc(z) = { cheta(z) } */
gfunc(z)=cheta(z);

gtaylor( w,r,samples) = {
  local(rinv,s,t,x1,y,z,tot,t_est,tcrc,halfsamples,wtaylor,terms);
/* outputs gie taylor series, the complex taylor series for sexp, */
/* default halfsamples=240 */
  if (samples==0, samples=240);  /* no matter how many sample points, the default gie series size is 200 halfsamples */
  halfsamples=samples/2;
  terms = floor(samples*200/240);
  t_est    = vector (samples,i,0);
  tcrc     = vector (samples,i,0);
  if (r==0,r=1);
  rinv = 1/r;
  wtaylor=0;
  for(s=1, samples, x1=-1/(samples)+(s/halfsamples); tcrc[s]=exp(Pi*I*x1); );

  /* uses the gfunc to allow for centering anywhere in the complex plane */
  for (t=1,samples, t_est[t] = gfunc(w+r*tcrc[t]); );

  for (s=0,terms-1,
    tot=0;
    for (t=1,samples,
      tot=tot+t_est[t];
      t_est[t]=t_est[t]*conj(tcrc[t]);
    );
    tot=tot/samples;
    if (s>=1, tot=tot*(rinv)^s);
    wtaylor=wtaylor+tot*x^s;
  );
  wtaylor=precision(wtaylor,precis);
  return(wtaylor);
}

invgfunc(z,est) = {
  local (y,s,slop,lastyz,curyz,lest,ly,pgoal);
/* inverse sexp(z), using "est" as initial estimate */
/* est is an optional parameter, important near the singularity */
/* the 0.01 radius used for the slope means won't converge within 0.01 of singularity */
  lastyz=100;
  y=gfunc(est);
  curyz=abs(y-z);
  lest=est+curyz*0.05;
  ly=gfunc(lest);
  pgoal=10^(-precis/1.3);
  /* generate the fixed point for pentation by iteration slog */
  s=1;
  while ((curyz>pgoal) && ((curyz<lastyz) || (s<3)),
    est=precision(est,precis);
    y=precision(y,precis);
    slop=(y-ly)/(est-lest);
    lest=est;
    ly=y;
    est=est+(z-y)/slop;
    lastyz=curyz;
    y=gfunc(est);
    curyz=abs(y-z);
    s++;
  );
  if (curyz>0.1, print (curyz " bad result, need better initial est, slog(z,est)"));
  return(est);
}

invlambertw(w)=w*exp(w);
/* lambertL(B) calculates the fixed point for base=B via lambert W function */
lambertL(B,est) = {est=-est*log(B);if ((est==0) && (real(B)>exp(1/exp(1))), est=-I);-wlambert(-log(B),est)/log(B);};

/* http://math.stackexchange.com/questions/420119/lambert-function-approximation-w-0-branch */
wlambert(z,est) = {
  local (y,s,slop,lastyz,curyz,lest,ly,pgoal);
/* inverse sexp(z), using "est" as initial estimate */
/* est is an optional parameter, important near the singularity */
  if ((est==0) && (real(-z)>1/exp(1)), est=-I);
  lastyz=100;
  curyz=10;
  pgoal=10^(-precis/1.3);
  /* generate the fixed point for pentation by iteration slog */
  s=1;
  while ((curyz>pgoal) && ((curyz<lastyz) || (s<3)),
    est=precision(est,precis);
    lest=est;
    est=(z*exp(-est)+est^2)/(est+1);
    curyz=abs(lest-est);
    s++;
  );
  if (curyz>0.1, print (curyz " bad result, need better initial est)"));
  return(est);
}

loop(t) = {
  local (rt,st,s,xmlt,y,precbits,lastloop,slowprecision);
  if (B==0, rawinit(exp(1))); /* avoids user errors, initializes for base(e) */
  if (initxsuperf==0,
    initsuperf(2*floor(precis*0.9)); /* initialize Schroder/iSchroder series */
  );
  if (loopcount>0, t=t+loopcount);
  lastloop=0;
  if ((loopcount==0),
    if (t==1, lastloop=1); /* t=1, only one loop */
    loopcount++;
    /* this is the initialization loop, use a reasonable set of initial values */
    thetaup(12); /* calculate a 12 term rie array from the initial rtheta approximation */
    sexpup(12); /* calculate a 12 term sie array from the thetaaprx */
    recenterup(); /* recenter and renormzlize the sie array */
    lastprecision=0;
    curprecision = erroravg(); /* calculate the current error term, to be used in the next iteration */
  );
  precbits=0;
  while ((lastloop==0),
    if (((curprecision-0.01)<lastprecision) && (curprecision<goalbits),  /* if curprecision>goalbits, who cares? */
      loopcount=-1;
      print ("UNEXPECTED LOSS: curprecision)<lastprecision. EXITING " curprecision);
      return(0);
/*    lastprecision=0; */
/*    curprecision=curprecision-8; */
    );
    loopcount++;
    if (loopcount==t, lastloop=1);
    precbits = curprecision+16;
    xmlt=loopradius; /* 0.5 is default radius in the loop */
    if (curprecision>goalbits, /* if-then-else */
      lastloop=1;                   /* IF: last iteration */
      xmlt=lastradius,              /* IF: use 0.65 for last iteration, and don't update precision */
    );
    /* st will become the new number of terms and sample points in the sexp taylor series array */
    y = precbits*xmlt + 1.5 + log(abs( polcoeff(xsexp,length(xsexp)-1) ))/log(2);
    st = length(xsexp) + floor(y);
    if (st<12, st=12);
    /* determine the value to update for the number of points in the thetaaprx taylor series */
    rt = floor((log(2)/log(imult))*(precbits - 2.5 + log(abs(polcoeff(xtheta,length(xtheta)-1)))/log(2)));
    thetaup(rt);
    sexpup(st);
    recenterup(); /* recenter and renormzlize the sie array */
    lastprecision=curprecision;
    curprecision = erroravg(); /* calculate the current error term, to be used in the next iteration */
    if (debugloop,
      if (loopcount>=debugloop,
      /*ploth(t=-0.6,0.6,x=t+idelta*1.1;y=sexp(x)-thetaaprx(x);[real(y),imag(y)]);*/
      /*ploth(t=-0.6,0.6,x=t+idelta*1.1;y=sexp(x)-thetaaprx(x);z=isuperf(sexp(x))-isuperf(thetaaprx(x));[real(y),imag(y),real(z),imag(z)]); */
        ploth(t=-0.6,0.6,x=t+idelta*1.0;y=sexp(x)-thetaaprx(x);[real(y),imag(y)]);
      /*ploth(t=-0.6,0.6,x=t+idelta*1.0;z=isuperf(sexp(x))-isuperf(thetaaprx(x));[real(z),imag(z)]);*/
      /*ploth(t=-0.6,0.6,x=t+idelta*1.1;z=isuperf(sexp(x))-isuperf(thetaaprx(x));[real(z),imag(z)]);*/
      );
    );
  );
  if ((precis>67) && (quietmode==0), default(format, "g0.64"));
  return(1);
}

base_e_2_10() = {
  init(exp(1));
  dumparray();
  init(2);
  dumparray();
  init(10);
  dumparray();
}

printtime(w) = {
  w=gettime/1000+0.005;
  print1("time "floor(w)".");
  w=(w-floor(w))*10;
  print1(floor(w));
  w=(w-floor(w))*10;
  print(floor(w));
}

dumparray(w) = {
  local (r,s,t);
  default(format, "g0.10");
  print ("sexp Taylor series approximation terms");
  if (centerat<>0, print ("sexp Tayler series centerat " centerat));
  default(format, "g0.64");
  write ("kneser.txt", " ");
  write ("kneser.txt", "base          " B);
  write ("kneser.txt", "fixed point   " L);
  write ("kneser.txt", "Pseudo Period " Period);
  if (centerat<>0, write ("kneser.txt", "sexp Tayler series centerat " centerat));
  write ("kneser.txt", " ");
  write ("kneser.txt", "iterations  " scnt " used for superf/isuperf");
  write ("kneser.txt", curprecision " theta/sexp binary precision bits");
  write ("kneser.txt", loopcount "=loopcount " recenter " recenter/renorm " renorm);
  write ("kneser.txt", " ");
  write ("kneser.txt", "xsexp sexp polynomial Taylor series");
  if (loopcount==-1, write ("kneser.txt", "UNEXPECTED PRECISION LOSS.  EXITED EARLY, curprecision<lastprecision " curprecision));
  for (t=1, length(xsexp), write  ("kneser.txt", t-1 " " polcoeff(xsexp,t-1)));
  write ("kneser.txt", " ");
  write ("kneser.txt", "theta approximation imaginary delta " idelta);
  write ("kneser.txt", "xtheta theta / 1-cyclic fourier series");
  for (t=1, length(xtheta), write  ("kneser.txt", t-1" " polcoeff(xtheta,t-1)));
  print ("kneser.txt  see file for theta and sexp Taylor approximation arrays");

  /* other interesting or semi-interesting plots              */
  /* ploth (t=-0.6,0.6, z=t+idelta;z=sexp(z)-thetaaprx(z);[real(z), imag(z)]  ); */
  /* print ("plot1, difference Riemann sexp approximation"); */
  /* ploth (t=-1,  1, sexp(t)                          ); */
  /* print ("plot2, sexp, Taylor series approximation"); */
  /* print ("plot3, log_2() Riemann approximation array, shows precision"); */
  /* ploth (t=-1.01, 1.011, z=thetaaprx(t); [real(z), imag(z)]); */
  /* ploth (t=-1.01, 0.011, imag(thetaaprx(t))          );     */
  /* ploth (t=-1.01, 0.011, imag(thetaaprx(t+idelta))   );     */
  /* ploth (t=-1.01, 0.011, imag(thetaaprx(t)-sexp(t))  );     */
  /* ploth (t=-1.01, 0.011, z=t+0.16*I;z=sexp(z)-thetaaprx(z);[real(z), imag(z)] );  */
}

help() = {
  print ("");
  print ("help menu for kneser.gp tetration program, 'morestuff()' for second menu");
  print ("");
  print ("init(base)   initialize and generate kneser mapping for base. init; uses base=e");
  print ("sexp(z)      returns sexp(z), evaluated via the taylor series approximations");
  print ("slog(z,est)  inverse sexp(z), using optional "est" as initial estimate");
  print ("superf(z)    complex superfunction of z, base B");
  print ("isuperf(z)   inverse complex superfunction of z, base B");
  print ("cheta(z)     base eta functions: cheta(z) sexpeta(z) invcheta(z) invsexpeta(z)");
  print ("B, L, Period base, fixedpoint, period of superf(z)=pseudo period of sexp(z)");
  print ("prtpoly(anypolynomial) pretty prints the polynomial series, see list below");
  print ("internal polynomials:  xtheta(x)       polynomial series used for thetaaprx(z)");
  print ("                       xsexp(x)        polynomial series used for sexp(z)");
  print ("                       iSchroder(x)    polynomial series used by superf(z)");
  print ("                       Schroder(x)     polynomial series used by isuperf(z)");
  print ("                       sexptaylor(w,r) sexp polynomial centered at w, radius r");
  print ("                       slogtaylor(w,r) slog polynomial centered at w, radius r");
  print ("");
  print ("quietmode=1; suppresses output during init, quietmode=0 default to allow output");
  print ("help()       print this menu,  'morestuff()'  for more special functions");
/*print ("loop or loop(0), loop until optimal precision, ~110 bits, takes ~4-5 seconds");*/
}

morestuff() = {
  print ("");
  print ("morestuff()   morestuff menu for Kneser.gp program");
  print ("functions");
  print ("sexpup;       updates the sexp(z) function xsexp from thetaaprx(z) function");
  print ("thetaup;      updates the thetaaprx(z) function xtheta from sexp(z) function");
  print ("recenterup;   updates the recenter/renorm values, based on renormup");
  print ("rawinit(base) initializes program for base, without looping; default base e");
  print ("loop(n)       n iteration loops, each loop adds 7-8 bits precision");
  print ("initcheta     only required when precision is manually changed, and no init(B)");
  print ("dumparray     dumps xsexp/xtheta Taylor series arrays to kneser.txt file");
  print ("base_e_2_10   initializes and writes series to kneser.txt for base e, 2, 10");
  print ("other functions");
  print ("wlambert(z,e) lambertL(b,est) uses wlambert to calc L");
  print ("gfunc(z)      set to whatever you want, gtaylor(z,r) generates polynomial");
  print ("superf2(z)    isuperf2(z), only for B<eta, sexp(z) from attracting fixed point");
  print ("thetaaprx(z)  superf(z+theta(z)) approx of sexp(z) accurate when imag(z)>0.012i");
  print ("             ");
  print ("centerat=0;   default center, other real values are used for larger bases");
  print ("gennewsexp=1  for B<etaB, generate the kneser mapping for newsexp");
  print ("gennewsexp=0  for B<etaB, generate the kneser mapping for sexp");
  print ("\\p 67         default 67 digits initial precision, allows sexp to 32 digits");
  print ("\\p 134 \\p 28  higher precision to 134 digits.  very fast precision 28 digits");
  print ("");
}


{
  initcheta();
  help();
}

