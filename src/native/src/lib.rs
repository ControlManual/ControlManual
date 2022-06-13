use pyo3::prelude::*;
mod bindings;

use bindings::py_tokenize;

#[pymodule]
fn _controlmanual(_py: Python<'_>, m: &PyModule) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(py_tokenize, m)?)?;
    Ok(())
}
