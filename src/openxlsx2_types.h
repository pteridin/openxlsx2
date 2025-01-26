/******************************************************************************
 *                                                                            *
 * This file defines typedefs. R expects it to be called <pkgname>_types.h    *
 *                                                                            *
 ******************************************************************************/

/* create custom Rcpp::wrap function to be used with std::vector<xml_col> */
#include <RcppCommon.h>

typedef struct {
  std::string r;
  std::string row_r;
  std::string c_r;     // CellReference
  std::string c_s;     // StyleIndex
  std::string c_t;     // DataType
  std::string c_cm;    // CellMetaIndex
  std::string c_ph;    // ShowPhonetic
  std::string c_vm;    // ValueMetaIndex
  std::string v;       // CellValue
  std::string f;       // CellFormula
  std::string f_attr;
  std::string is;      // inlineStr
} xml_col;

typedef std::vector<std::string> vec_string;

// matches openxlsx2_celltype in openxlsx2.R
enum celltype {
  short_date     = 0,
  long_date      = 1,
  numeric        = 2,
  logical        = 3,
  character      = 4,
  formula        = 5,
  accounting     = 6,
  percentage     = 7,
  scientific     = 8,
  comma          = 9,
  hyperlink      = 10,
  array_formula  = 11,
  factor         = 12,
  string_num     = 13,
  cm_formula     = 14,
  hms_time       = 15,
  currency       = 16,
  list           = 17
};

// check for 1.0.8.0
#if RCPP_DEV_VERSION >= 1000800
#include <Rcpp/Lightest>
#else
#include <Rcpp.h>
#endif

// custom wrap function
// Converts the imported values from c++ std::vector<xml_col> to an R dataframe.
// Whenever new fields are spotted they have to be added here
namespace Rcpp {
template <>
inline SEXP wrap(const std::vector<xml_col>& x) {
  R_xlen_t n = static_cast<R_xlen_t>(x.size());

  // Vector structure identical to xml_col from openxlsx2_types.h
  Rcpp::CharacterVector r(no_init(n));         // cell name: A1, A2 ...
  Rcpp::CharacterVector row_r(no_init(n));     // row name: 1, 2, ..., 9999

  Rcpp::CharacterVector c_r(no_init(n));       // col name: A, B, ..., ZZ
  Rcpp::CharacterVector c_s(no_init(n));       // cell style
  Rcpp::CharacterVector c_t(no_init(n));       // cell type
  Rcpp::CharacterVector c_cm(no_init(n));
  Rcpp::CharacterVector c_ph(no_init(n));
  Rcpp::CharacterVector c_vm(no_init(n));

  Rcpp::CharacterVector v(no_init(n));         // <v> tag
  Rcpp::CharacterVector f(no_init(n));         // <f> tag
  Rcpp::CharacterVector f_attr(no_init(n));    // <f /> attributes
  Rcpp::CharacterVector is(no_init(n));        // <is> tag

  // struct to vector
  // We have to convert utf8 inputs via Rcpp::String for non unicode R sessions
  // Ideally there would be a function that calls Rcpp::String only if needed
  for (R_xlen_t i = 0; i < n; ++i) {
    size_t ii = static_cast<size_t>(i);
    if (!x[ii].r.empty())      r[i]      = std::string(x[ii].r);
    if (!x[ii].row_r.empty())  row_r[i]  = std::string(x[ii].row_r);
    if (!x[ii].c_r.empty())    c_r[i]    = std::string(x[ii].c_r);
    if (!x[ii].c_s.empty())    c_s[i]    = std::string(x[ii].c_s);
    if (!x[ii].c_t.empty())    c_t[i]    = std::string(x[ii].c_t);
    if (!x[ii].c_cm.empty())   c_cm[i]   = std::string(x[ii].c_cm);
    if (!x[ii].c_ph.empty())   c_ph[i]   = Rcpp::String(x[ii].c_ph);
    if (!x[ii].c_vm.empty())   c_vm[i]   = std::string(x[ii].c_vm);
    if (!x[ii].v.empty()) { // can only be utf8 if c_t = "str"
      if (x[ii].c_t.empty() && x[ii].f_attr.empty())
        v[i] = std::string(x[ii].v);
      else
        v[i] = Rcpp::String(x[ii].v);
    }
    if (!x[ii].f.empty())      f[i]      = Rcpp::String(x[ii].f);
    if (!x[ii].f_attr.empty()) f_attr[i] = std::string(x[ii].f_attr);
    if (!x[ii].is.empty())     is[i]     = Rcpp::String(x[ii].is);
  }

  // Assign and return a dataframe
  return Rcpp::wrap(
    Rcpp::DataFrame::create(
      Rcpp::Named("r")      = r,
      Rcpp::Named("row_r")  = row_r,
      Rcpp::Named("c_r")    = c_r,
      Rcpp::Named("c_s")    = c_s,
      Rcpp::Named("c_t")    = c_t,
      Rcpp::Named("c_cm")   = c_cm,
      Rcpp::Named("c_ph")   = c_ph,
      Rcpp::Named("c_vm")   = c_vm,
      Rcpp::Named("v")      = v,
      Rcpp::Named("f")      = f,
      Rcpp::Named("f_attr") = f_attr,
      Rcpp::Named("is")     = is,
      Rcpp::Named("stringsAsFactors") = false
    )
  );
}

template <>
inline SEXP wrap(const vec_string& x) {
  R_xlen_t n = static_cast<R_xlen_t>(x.size());

  Rcpp::CharacterVector z(no_init(n));

  for (R_xlen_t i = 0; i < n; ++i) {
    z[i] = Rcpp::String(x[static_cast<size_t>(i)]);
  }

  return Rcpp::wrap(z);
}

}  // namespace Rcpp

// pugixml defines. This creates the xmlptr
#include "pugixml.hpp"

typedef pugi::xml_document xmldoc;
typedef Rcpp::XPtr<xmldoc> XPtrXML;
