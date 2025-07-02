import numpy as np
import re

def extract_traditional_features(filepath):
    """
    Extracts a 21-feature vector (McCabe & Halstead) from a C++ source code file.

    IMPORTANT: The order of features in the returned array MUST MATCH
    the order in your training .arff file. This example is based on the
    standard PROMISE repository order.

    Args:
        filepath (str): The path to the C++ source code file.

    Returns:
        numpy.ndarray: A 1x21 NumPy array containing the features.
    """
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            source_code = f.read()
    except Exception as e:
        print(f"Error reading file {filepath}: {e}")
        # Return a vector of zeros if file can't be read
        return np.zeros((1, 21))

    # Split into lines for analysis
    lines = source_code.split('\n')
    
    # --- 1. Raw Metrics (Lines of Code) ---
    loc = len(lines)  # Total lines of code
    blank_lines = sum(1 for line in lines if line.strip() == '')
    
    # Count comment lines (single line // and multi-line /* */)
    comment_lines = 0
    in_multiline_comment = False
    
    for line in lines:
        stripped = line.strip()
        if in_multiline_comment:
            comment_lines += 1
            if '*/' in line:
                in_multiline_comment = False
        elif stripped.startswith('//'):
            comment_lines += 1
        elif '/*' in line:
            comment_lines += 1
            if '*/' not in line:
                in_multiline_comment = True
    
    comments = comment_lines
    sloc = loc - blank_lines - comment_lines  # Source lines of code (non-empty, non-comment)
    lloc = sloc  # Logical lines of code (approximation)
    lOCodeAndComment = 0  # Mixed lines (approximation)

    # --- 2. McCabe's Cyclomatic Complexity (C++ approximation) ---
    # Count decision points: if, while, for, switch, case, catch, &&, ||, ?
    complexity_keywords = ['if', 'while', 'for', 'switch', 'case', 'catch']
    
    # Remove comments and strings for accurate counting
    code_without_comments = re.sub(r'//.*', '', source_code)
    code_without_comments = re.sub(r'/\*.*?\*/', '', code_without_comments, flags=re.DOTALL)
    code_without_strings = re.sub(r'"[^"]*"', '', code_without_comments)
    code_without_strings = re.sub(r"'[^']*'", '', code_without_strings)
    
    total_complexity = 1  # Base complexity
    branch_count = 0
    
    for keyword in complexity_keywords:
        # Use word boundaries to avoid partial matches
        pattern = r'\b' + keyword + r'\b'
        matches = len(re.findall(pattern, code_without_strings, re.IGNORECASE))
        total_complexity += matches
        branch_count += matches
    
    # Count logical operators
    logical_ops = len(re.findall(r'&&|\|\|', code_without_strings))
    total_complexity += logical_ops
    branch_count += logical_ops
    
    # Count ternary operators
    ternary_ops = len(re.findall(r'\?', code_without_strings))
    total_complexity += ternary_ops
    branch_count += ternary_ops

    v_g = total_complexity
    ev_g = total_complexity  # Essential complexity (approximation)
    iv_g = total_complexity  # Design complexity (approximation)

    # --- 3. Halstead Metrics (C++ approximation) ---
    # Define C++ operators and keywords
    cpp_operators = [
        '+', '-', '*', '/', '%', '=', '==', '!=', '<', '>', '<=', '>=',
        '&&', '||', '!', '&', '|', '^', '~', '<<', '>>', '++', '--',
        '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=',
        '->', '.', '::', '?', ':', ',', ';', '(', ')', '[', ']', '{', '}',
        'new', 'delete', 'sizeof', 'typeof'
    ]
    
    cpp_keywords = [
        'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do',
        'double', 'else', 'enum', 'extern', 'float', 'for', 'goto', 'if',
        'int', 'long', 'register', 'return', 'short', 'signed', 'sizeof',
        'static', 'struct', 'switch', 'typedef', 'union', 'unsigned', 'void',
        'volatile', 'while', 'class', 'private', 'public', 'protected',
        'virtual', 'friend', 'inline', 'operator', 'this', 'new', 'delete',
        'bool', 'true', 'false', 'namespace', 'using', 'try', 'catch', 'throw'
    ]
    
    # Count operators
    unique_operators = set()
    total_operators = 0
    
    for op in cpp_operators:
        escaped_op = re.escape(op)
        if op.isalpha():  # Keywords need word boundaries
            pattern = r'\b' + escaped_op + r'\b'
        else:
            pattern = escaped_op
        
        matches = len(re.findall(pattern, code_without_strings))
        if matches > 0:
            unique_operators.add(op)
            total_operators += matches
    
    # Count operands (identifiers, constants, string literals)
    # Simple approximation: words that are not keywords or operators
    words = re.findall(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', code_without_strings)
    numbers = re.findall(r'\b\d+\.?\d*\b', code_without_strings)
    
    operands = []
    for word in words:
        if word.lower() not in [kw.lower() for kw in cpp_keywords]:
            operands.append(word)
    operands.extend(numbers)
    
    unique_operands = set(operands)
    
    uniq_Op = len(unique_operators)
    uniq_Opnd = len(unique_operands)
    total_Op = total_operators
    total_Opnd = len(operands)
    
    # Halstead metrics calculations
    n = total_Op + total_Opnd  # Program length
    N = uniq_Op + uniq_Opnd    # Vocabulary
    
    if N > 0 and uniq_Op > 0 and uniq_Opnd > 0:
        v = n * np.log2(N) if N > 1 else 0  # Volume
        d = (uniq_Op / 2.0) * (total_Opnd / uniq_Opnd) if uniq_Opnd > 0 else 0  # Difficulty
        l = 1 / d if d > 0 else 0  # Level
        e = d * v  # Effort
        t = e / 18.0  # Time (assuming 18 mental discriminations per second)
        b = v / 3000.0  # Bugs (empirical constant)
        i = v / d if d > 0 else 0  # Intelligence
    else:
        v = l = d = i = e = b = t = 0

    # --- 4. Assemble the Feature Vector IN THE CORRECT ORDER ---
    # This order is based on the standard PROMISE repository order
    feature_vector = [
        loc,
        v_g,
        ev_g,
        iv_g,
        n,
        v,
        l,
        d,
        i,
        e,
        b,
        t,
        sloc,
        comments,
        blank_lines,
        lOCodeAndComment,
        uniq_Op,
        uniq_Opnd,
        total_Op,
        total_Opnd,
        branch_count
    ]

    # Return as a 2D NumPy array (1 row, 21 columns) for prediction
    return np.array(feature_vector, dtype=np.float32).reshape(1, -1)