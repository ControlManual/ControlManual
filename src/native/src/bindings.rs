use pyo3::prelude::*;
use pyo3::exceptions::PyValueError;

#[pyfunction]
#[pyo3(name = "tokenize")]
pub fn py_tokenize(data: &str) -> PyResult<Vec<String>> {
    let tokens = lexer::tokenize(data);

    match tokens {
        Err(e) => Err(PyValueError::new_err(e)),
        Ok(d) => Ok(d)
    }
}
