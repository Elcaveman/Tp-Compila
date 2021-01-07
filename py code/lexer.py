import re
import mmap 
from dataclasses import dataclass,field
#*from ErrorHandler import *
""" About Pascal:
    Grammar in pascal_grammar.txt
    https://www.tutorialspoint.com/pascal/
"""

class Token:
    _TOKENTYPES = [
        'com',#comment
        'lit',#literal
        'sep',#separator({},;begin end)
        'key',#global keyword
        'dt',#data type: we will only have standard:
        'op',#operator
        'id',#identifier
        #[int,float,char,bool] ,
        #pointers[declare:pt:^real , get vallue: pt^ ,get adress @variable] ,structured[array,string]
    ]
    def __init__(self,META,T_type,T_regex,T_name=''):
        if (T_type in Token._TOKENTYPES):
            self.__type = T_type   # private
            self._name = T_name
            self._regex = T_regex # protected
            self._hasValue = True if T_type in ['lit','id'] else False
            if (T_type in ['sep','key','op','dt']):
                META.RESERVED_SYMBOLS.append(T_regex)
        else:
            raise ValueError(f"{T_type} Invalid Token Type. Expected one of: {Token._TOKENTYPES}")
    def fullmatch(self,value):
        return re.compile(self._regex).fullmatch(value)
    def __repr__(self):
        return f'(type:{self.__type}, name:{self._name if self._name else self._regex})'

class Lexeme:
    def __init__(self,token : Token, span : tuple,value=''):
        self._token = token
        self._span = span
        if token._hasValue==True:
            #Test if the match found actually matches the Token!
            if self._token.fullmatch(value):
                self._value = value
            else:
                raise ValueError(f'The value {value} doesn\'t match the Token.')
        else:
            self._value = self._token._regex
    def __repr__(self):
        return f'(token:{self._token} , span:{self._span} ,description: {self._value})'

class Map(dict):
    def __init__(self,*args,**kwargs):
        self.update(*args,**kwargs)

    def __setitem__(self, key, item):
        self.__dict__[key] = item

    def __getitem__(self, key):
        return self.__dict__[key]

    def __repr__(self):
        return repr(self.__dict__)

    def __len__(self):
        return len(self.__dict__)

    def __delitem__(self, key):
        del self.__dict__[key]

    def clear(self):
        return self.__dict__.clear()

    def copy(self):
        return self.__dict__.copy()

    def has_key(self, k):
        return k in self.__dict__

    def update(self, *args, **kwargs):
        return self.__dict__.update(*args, **kwargs)

    def keys(self):
        return self.__dict__.keys()

    def values(self):
        return self.__dict__.values()

    def items(self):
        return self.__dict__.items()

    def pop(self, *args):
        return self.__dict__.pop(*args)

    def __cmp__(self, dict_):
        return self.__cmp__(self.__dict__, dict_)

    def __contains__(self, item):
        return item in self.__dict__

    def __iter__(self):
        return iter(self.__dict__)

    def __unicode__(self):
        pass

@dataclass
class LanguageMetaData:
    CASE_SENSITIVE : bool=field(default=True)
    MULTILINE_EXPRESSIONS : bool=field(default=False)
    MULTILINE_COMMENT :bool=field(default=True);

    LETTER : str = field(default='[_a-zA-Z]')
    DIGIT : str = field(default='[0-9]')
    LEGAL_WORD : str = field(default='[_a-zA-Z0-9&]')
    SIGN : str = field(default='[\+\-]')
    TOKEN_SEPARATORS :str = field(default='\s')
    REAL_SEP : str = field(default='\.')

    IDENTIFIER_MAXLEN : int=field(default=20)
    NUMBER_MAXLEN : int=field(default=11)
    LINE_MAXLEN : int=field(default=80)

    RESERVED_SYMBOLS: list =field(default_factory=list)

class Pascal():
    def __init__(self):
        self._META = LanguageMetaData(CASE_SENSITIVE=False)
        self._TokenMAP = Map(id=Map(),key=Map(),dt=Map(),sep=Map(),op=Map(),lit=Map(),com=Map())
        #! The order in which the tokens are added is the order in which they are matched
        self._TokenMAP.op.NOT_EQUAL = Token(self._META,'op',r"<>","not equal")
        self._TokenMAP.op.LE = Token(self._META,'op',r"<=","lesser or equal")
        self._TokenMAP.op.GE = Token(self._META,'op',r">=",'greater or equal')
        self._TokenMAP.op.NORMAL_ASSIGNMENT =Token(self._META,'op',r'\:=')
        self._TokenMAP.op.PLUS_ASSIGNMENT = Token(self._META,'op',r'\+=')
        self._TokenMAP.op.MINUS_ASSIGNMENT = Token(self._META,'op',r'-=')
        self._TokenMAP.op.MULT_ASSIGNMENT = Token(self._META,'op',r'\*=')
        self._TokenMAP.op.DIV_ASSIGNMENT = Token(self._META,'op',r'/=')

        self._TokenMAP.op.OR = Token(self._META,'op',r'or')
        self._TokenMAP.op.NOT = Token(self._META,'op',r'not')
        self._TokenMAP.op.AND = Token(self._META,'op',r'and')
        self._TokenMAP.op.XOR = Token(self._META,'op',r'xor')
        self._TokenMAP.op.MOD = Token(self._META,'op',r'mod') 

        self._TokenMAP.op.ADRESS = Token(self._META,'op',r'\@')
        self._TokenMAP.op.POINTER = Token(self._META,'op',r'\^')
        self._TokenMAP.op.PLUS = Token(self._META,'op',r'[+]')
        self._TokenMAP.op.MINUS = Token(self._META,'op',r'[-]')
        self._TokenMAP.op.MULT = Token(self._META,'op',r'[*]')
        self._TokenMAP.op.DIV = Token(self._META,'op',r'[/]')
        self._TokenMAP.op.EQUAL = Token(self._META,'op',r'\=')
        self._TokenMAP.op.LT = Token(self._META,'op',r'\<')
        self._TokenMAP.op.GT = Token(self._META,'op',r'\>')
        self._TokenMAP.op.COLON = Token(self._META,'op',r'\:')

        self._TokenMAP.sep.BEGIN = Token(self._META,'sep',r'begin')
        self._TokenMAP.sep.END = Token(self._META,'sep',r'end')
        self._TokenMAP.sep.COMA = Token(self._META,'sep',r'\,')
        self._TokenMAP.sep.PT = Token(self._META,'sep',r'\.')
        self._TokenMAP.sep.SEMICOLON = Token(self._META,'sep',r'\;')
        self._TokenMAP.sep.OPEN_PARENTHESIS = Token(self._META,'sep',r'\(')
        self._TokenMAP.sep.CLOSE_PARENTHESIS =Token(self._META,'sep',r'\)')
        self._TokenMAP.sep.OPEN_BRAKET = Token(self._META,'sep',r'\[')
        self._TokenMAP.sep.CLOSE_BRAKET = Token(self._META,'sep',r'\]')
        
        self._TokenMAP.sep.OP_ONELINE_COM = Token(self._META,'sep',r'{')
        self._TokenMAP.sep.CLS_ONELINE_COM = Token(self._META,'sep',r'}')
        self._TokenMAP.sep.OP_MLINE_COM = Token(self._META,'sep',r'\(\*')
        self._TokenMAP.sep.CLS_MLINE_COM = Token(self._META,'sep',r'\*\)')

        self._TokenMAP.key.FUNCTION = Token(self._META,'key',r'function')
        self._TokenMAP.key.PROCEDURE = Token(self._META,'key',r'procedure')
        self._TokenMAP.key.IF = Token(self._META,'key','if')
        self._TokenMAP.key.PROGRAM = Token(self._META,'key',r'program')
        self._TokenMAP.key.USES = Token(self._META,'key',r'uses')
        self._TokenMAP.key.THEN = Token(self._META,'key',r'then')
        self._TokenMAP.key.WHILE = Token(self._META,'key',r'while')
        self._TokenMAP.key.FOR = Token(self._META,'key',r'for')
        self._TokenMAP.key.TO = Token(self._META,'key',r'to')
        self._TokenMAP.key.OF = Token(self._META,'key',r'of')
        self._TokenMAP.key.DO = Token(self._META,'key',r'do')
        self._TokenMAP.key.CONST = Token(self._META,'key',r'const')
        self._TokenMAP.key.VAR = Token(self._META,'key',r'var')
        self._TokenMAP.key.READ = Token(self._META,'key',r'read')
        self._TokenMAP.key.WRITE = Token(self._META,'key',r'write')
        #? add switch case afterwards        

        self._TokenMAP.dt.INT = Token(self._META,'dt',r'integer')
        self._TokenMAP.dt.FLOAT = Token(self._META,'dt',r'real')
        self._TokenMAP.dt.CHAR = Token(self._META,'dt',r'char')
        self._TokenMAP.dt.BOOL = Token(self._META,'dt',r'boolean')
        self._TokenMAP.dt.STRING = Token(self._META,'dt',r'string')
        self._TokenMAP.dt.ARRAY = Token(self._META,'dt',r'array')#only the simple array declaration

        self._TokenMAP.id.IDENTIFIER = Token(
            self._META,'id',
            r''+f'{self._META.LETTER}{self._META.LEGAL_WORD}{{0,{self._META.IDENTIFIER_MAXLEN}}}'
        )
        #?all tokens are ordered by level of importance (which one matches first)

        if self._META.MULTILINE_EXPRESSIONS:
            self._TokenMAP.lit.STRING_LITERAL =  Token(
            self._META,'lit',
            r"""(?:"[^"']*(?:[^"']*)")|(?:'[^"']*(?:[^"']*)')"""
            )
        else:
            self._TokenMAP.lit.STRING_LITERAL =  Token(self._META,'lit',r"""(?:"[^'"]*"|'[^'"]')""")
        
        self._TokenMAP.lit.FLOAT_LITERAL = Token(
            self._META,'lit',
            r''+f'{self._META.DIGIT}{{1,{self._META.NUMBER_MAXLEN}}}{self._META.REAL_SEP}{self._META.DIGIT}{{1,{self._META.NUMBER_MAXLEN}}}'
        )

        self._TokenMAP.lit.INT_LITERAL = Token(
            self._META,'lit',
            r''+f'[1-9][0-9]{{1,{self._META.NUMBER_MAXLEN}}}|0'
        )
        
        self._TokenMAP.com.MULTLINE_COMMENT =  Token(self._META,'com',r"""\(\*[^*]*\*+(?:[^)*][^*]*\*+)*\)""")
        self._TokenMAP.com.ONELINE_COMMENT =  Token(self._META,'com',r"{.*}")

        self._CAPTURE_STRING = self.init_CaptureString()
        if self._META.CASE_SENSITIVE:
            self._COMPILED_CS = re.compile(self._CAPTURE_STRING)
        else:
            self._COMPILED_CS = re.compile(self._CAPTURE_STRING,re.IGNORECASE)

    def init_CaptureString(self):
        CAPTURE_STRING = r'(?:'#(?:(?<token_1>regex1)|(?<token_2>regex2)...) (?:non cap) (?<grpname>cap)
        for name in Token._TOKENTYPES:
            for T_type,token in self._TokenMAP[name].items():
                CAPTURE_STRING += f'(?P<{name}___{T_type}>{token._regex})|'
        CAPTURE_STRING+=f'(?P<Forbiden>[^{self._META.TOKEN_SEPARATORS}])'
        CAPTURE_STRING += ')'
        return CAPTURE_STRING

    def Tokenize(self,filePath,debug=0):
        LEXEMIST = []
        if (filePath.endswith('pas')==False):
            raise FileNotFoundError('Chose a File with .pas Extension.')
        
        with open(filePath,'r',encoding='utf-8') as file:
            ite = self._COMPILED_CS.finditer(file.read())
            for match in ite:
                if debug==1:print(match)
                for name_t,value in match.groupdict().items():
                    try:
                        typ,token_name = name_t.split("___")
                    except ValueError:
                        typ = name_t
                    if value!=None:
                        if typ == 'Forbiden':
                            raise Exception("Syntax Error",f"Invalid Token:{value} at {match.span()}")
                        else:
                            LEXEMIST.append(
                            Lexeme(self._TokenMAP[typ][token_name],match.span(),value))
            #?use mmap to optimise the size of the files_read and time complexity using multiprocessing (handled by mmap)
            # # with mmap.mmap(file.fileno(), 0) as m:
            # #     pass
        return LEXEMIST

class Test:
    def __init__(self,LanguageClass):
        self.lang = LanguageClass()

    def lexer_test(self,filename):
        return self.lang.Tokenize(filename,debug=1)


if __name__ == '__main__':
    Test_Files=[
        'D:\\Daoud\\Progs for fun\\Projex\\Ensias Projects\\Simple Pascal compiler\\text.pas'
    ]
    for file in Test_Files:
        x = Test(Pascal)
        lexem_list = x.lexer_test(file)
        print('\n')
        for element in lexem_list:
            print(element)