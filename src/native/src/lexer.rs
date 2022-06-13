enum TokenType {
    LITERAL,
    KEYWORD
}

pub struct Token {
    data: String,
    token_type: TokenType,
}

fn split(data: &str) -> Result<Vec<String>> {
    let mut tokens: Vec<String> = Vec::new();
    let mut squote = false;
    let mut dquote = false;
    let mut parens = 0;

    for i in data {
        if parens < 0 {
            return Err("unexpected \")\"")
        }

        if i == ' ' && !squote && !dquote && !parens {
            tokens.push("");
            continue;
        }

        parens += if i == '(' { 1 } else { if i == ')' { -1 } else { 0 } };
        squote = if i == '\'' { !squote } else { squote };
        dquote = if i == '\'' { !dquote } else { dquote };

        tokens[-1] += i;
    }

    if (squote || dquote) {
        return Err("unmatched quotations")
    }

    if (parens) {
        return Err("expected \")\"");
    }

    Ok(tokens)
}

pub fn tokenize(data: &str) -> Result<Vec<Token>> {
    let split = match split(data) {
        Ok(data) => data,
        Err(e) => return Err(e)
    };

    let mut tokens: Vec<Token> = Vec::new();

    for string in split {
        
    }

    Ok(tokens)
}
