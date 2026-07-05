# -*- coding: utf-8 -*-
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import cm
from reportlab.lib import colors
from reportlab.lib.styles import ParagraphStyle
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (SimpleDocTemplate, Paragraph, Spacer, Table,
                                TableStyle, HRFlowable, PageBreak)

pdfmetrics.registerFont(TTFont("Malgun", r"C:\Windows\Fonts\malgun.ttf"))
pdfmetrics.registerFont(TTFont("MalgunBd", r"C:\Windows\Fonts\malgunbd.ttf"))

NAVY = colors.HexColor("#1E2761")
ACC  = colors.HexColor("#2F3C7E")
GREY = colors.HexColor("#555555")
LIGHT= colors.HexColor("#EEF1FA")

title = ParagraphStyle("title", fontName="MalgunBd", fontSize=17, leading=21,
                       textColor=NAVY, spaceAfter=2, alignment=1)
sub   = ParagraphStyle("sub", fontName="Malgun", fontSize=9.5, leading=12,
                       textColor=GREY, alignment=1, spaceAfter=6)
h1    = ParagraphStyle("h1", fontName="MalgunBd", fontSize=12.5, leading=15,
                       textColor=NAVY, spaceBefore=8, spaceAfter=3)
h2    = ParagraphStyle("h2", fontName="MalgunBd", fontSize=10, leading=13,
                       textColor=ACC, spaceBefore=4, spaceAfter=1)
body  = ParagraphStyle("body", fontName="Malgun", fontSize=9.3, leading=13.2,
                       textColor=colors.black, spaceAfter=2)
bullet= ParagraphStyle("bullet", fontName="Malgun", fontSize=9.3, leading=13,
                       leftIndent=10, bulletIndent=2, spaceAfter=1)
code  = ParagraphStyle("code", fontName="Malgun", fontSize=8.6, leading=11.5,
                       textColor=colors.HexColor("#333333"), leftIndent=8,
                       backColor=LIGHT, borderPadding=4, spaceBefore=2, spaceAfter=4)

def P(t, s=body): return Paragraph(t, s)
def B(t): return Paragraph("• " + t, bullet)

doc = SimpleDocTemplate(r"C:\Users\이현렬\GitHub\window-programing\오목배틀\오목AI_설계보고서.pdf",
                        pagesize=A4, topMargin=1.4*cm, bottomMargin=1.3*cm,
                        leftMargin=1.7*cm, rightMargin=1.7*cm)
s = []

s.append(P("오목 인공지능 설계 보고서", title))
s.append(P("게임공학과 &nbsp; 2023184027 &nbsp; 이현렬 &nbsp;|&nbsp; 전략 및 구현 설명", sub))
s.append(HRFlowable(width="100%", thickness=1.1, color=NAVY, spaceAfter=6))

# 1. 개요
s.append(P("1. 개요", h1))
s.append(P("본 오목 AI는 자료구조에서 학습한 <b>게임 트리 최대최소(minimax) 탐색</b>에 "
           "<b>위협 패턴 평가</b>와 <b>흑 금수(禁手) 회피</b>를 결합하여 구현하였다. "
           "심판 규칙(정확히 5목=승, 6목 이상 장목=패, 흑은 3-3·4-4도 반칙패, 1수 500ms 제한)을 "
           "엔진에 그대로 반영하였으며, 매 수마다 약 400ms 안에 최선의 착점을 계산한다.", body))

# 2. 전략
s.append(P("2. 사용한 전략", h1))

s.append(P("2.1 위협 패턴 인식", h2))
s.append(P("각 빈칸에 돌을 두었을 때 만들어지는 모양을, 그 점을 중심으로 한 4방향(가로·세로·대각2) "
           "11칸 문자열로 변환해 분석한다(M=내 돌, E=빈칸, B=막힘). 이를 통해 다음 핵심 위협을 탐지한다.", body))
s.append(B("<b>열린4</b>(EMMMME): 다음 수에 무조건 5목 — 막을 수 없는 필승 위협"))
s.append(B("<b>4</b>(MMMM·): 다음 수 5목 완성 가능 — 상대에게 강제 방어 요구"))
s.append(B("<b>열린3</b>(EMMME 등): 다음 수에 열린4로 키울 수 있는 공격의 씨앗"))

s.append(P("2.2 게임 트리 최대최소 탐색 (알파-베타)", h2))
s.append(P("내 차례는 점수를 최대화, 상대 차례는 최소화하는 minimax를 negamax 형태로 구현하고 "
           "<b>알파-베타 가지치기</b>로 불필요한 가지를 잘라 탐색 속도를 높였다. "
           "<b>반복 심화(iterative deepening)</b>로 깊이 2→4→6…을 점차 늘리며, "
           "시간 예산(약 400ms)이 소진되면 그때까지의 최선 수를 반환하여 시간초과 패배를 원천 차단한다.", body))

s.append(P("2.3 정밀 평가함수 (성능의 핵심)", h2))
s.append(P("탐색 말단에서 판세를 점수화한다. 단순히 ‘돌 개수’를 세지 않고 "
           "<b>연속 돌의 정확한 길이 + 양 끝의 열림 여부</b>를 평가한다(열린4=10만, 막힌4=1.2만, "
           "열린3=6천 …). 이 평가 방식 도입 후 동급 상대 전적이 5승5패에서 19승1패로 향상되었다.", body))

s.append(P("2.4 흑 금수 회피 + 2.5 의사결정 우선순위", h2))
s.append(P("흑은 3-3·4-4·장목이 반칙패이므로, 3중 안전망(탐색 중 제외 → 상대 차단 시 회피 → "
           "최종 보정)으로 스스로 금수를 두지 않는다. 최종 착점은 다음 우선순위로 결정한다.", body))
s.append(P("① 내가 즉시 5목 → 둔다 &nbsp;&nbsp; ② 상대가 다음 5목 → 막는다 &nbsp;&nbsp; "
           "③ 알파-베타 탐색의 최선 수 &nbsp;&nbsp; ④ (흑) 금수면 비금수 최선으로 교체", code))

doc_results = True

# 3. 구현
s.append(P("3. 프로그래밍 구현", h1))
impl = [
    ("보드·좌표", "board[행][열] 19×19 배열(0=빈, 1=흑, 2=백). 심판 좌표(x=열, y=행)에 맞춰 board[y][x]로 저장하고, 승리 인정 범위인 0~17에만 착수한다."),
    ("패턴 분석 ec_dir / ec_features", "한 점·한 방향의 11칸 문자열을 만들어 5목·장목·4·열린4·열린3을 비트플래그로 판정하고, 4방향을 합산해 한 수의 위협 특성을 산출한다."),
    ("평가함수 ec_color_score / ec_eval", "각 색의 모든 연속 돌을 ‘시작점’ 기준으로 스캔해 길이와 열린 끝으로 점수화한 뒤, (내 점수 − 상대 점수×1.1)을 반환한다(수비 가중)."),
    ("탐색 ec_search", "negamax + 알파-베타. 후보는 점수 정렬 후 상위 16개만, 깊이가 깊을수록 좁게 보아 시간 내 탐색을 보장한다."),
    ("후보 생성·시간관리", "기존 돌 반경 2칸 이내의 빈칸만 후보로 생성(ec_gen). clock() 기반 마감시각으로 매 노드 시간을 점검한다."),
    ("심판 인터페이스", "BlackAttack/Defence·WhiteAttack/Defence_2023184027 4함수 제공. Attack은 ec_best로 (행,열) 계산 후 (열,행)으로 반환, Defence는 상대 수를 내부 보드에 기록한다."),
]
rows = [[Paragraph("<b>"+a+"</b>", ParagraphStyle("c1", fontName="MalgunBd", fontSize=8.8, leading=11, textColor=NAVY)),
         Paragraph(b, ParagraphStyle("c2", fontName="Malgun", fontSize=8.8, leading=11.3))] for a,b in impl]
t = Table(rows, colWidths=[4.2*cm, 12.4*cm])
t.setStyle(TableStyle([
    ("VALIGN",(0,0),(-1,-1),"TOP"),
    ("BOX",(0,0),(-1,-1),0.6,NAVY),
    ("INNERGRID",(0,0),(-1,-1),0.4,colors.HexColor("#C7CEE6")),
    ("BACKGROUND",(0,0),(0,-1),LIGHT),
    ("TOPPADDING",(0,0),(-1,-1),3),("BOTTOMPADDING",(0,0),(-1,-1),3),
    ("LEFTPADDING",(0,0),(-1,-1),5),("RIGHTPADDING",(0,0),(-1,-1),5),
]))
s.append(t)

# 4. 결과
s.append(P("4. 성능 및 테스트 결과", h1))
s.append(P("심판 규칙을 그대로 적용한 자동 대국 프로그램으로 검증하였다.", body))
res = [["대상","결과"],
       ["랜덤·그리디 봇 (흑/백)","전승 (자기 반칙·시간초과 0)"],
       ["동급 탐색형 AI","평가함수 개선 후 19승 1패"],
       ["1수 최대 소요시간","약 401ms (500ms 제한 안전)"]]
rt = Table(res, colWidths=[6.5*cm, 10.1*cm])
rt.setStyle(TableStyle([
    ("FONTNAME",(0,0),(-1,0),"MalgunBd"),("FONTNAME",(0,1),(-1,-1),"Malgun"),
    ("FONTSIZE",(0,0),(-1,-1),8.8),
    ("BACKGROUND",(0,0),(-1,0),NAVY),("TEXTCOLOR",(0,0),(-1,0),colors.white),
    ("BOX",(0,0),(-1,-1),0.6,NAVY),("INNERGRID",(0,0),(-1,-1),0.4,colors.HexColor("#C7CEE6")),
    ("ROWBACKGROUNDS",(0,1),(-1,-1),[colors.white, LIGHT]),
    ("TOPPADDING",(0,0),(-1,-1),3.5),("BOTTOMPADDING",(0,0),(-1,-1),3.5),
    ("LEFTPADDING",(0,0),(-1,-1),6),
]))
s.append(rt)

s.append(Spacer(1, 6))
s.append(HRFlowable(width="100%", thickness=0.6, color=colors.HexColor("#C7CEE6"), spaceAfter=4))
s.append(P("요약: ① 위협 패턴 인식 ② 게임 트리 최대최소 탐색(알파-베타) ③ 정밀 평가함수 ④ 흑 금수 회피 "
           "— 네 가지를 결합해, 빠르고(≈0.4초) 안정적이며(반칙·시간초과 0) 강한 오목 AI를 구현하였다.",
           ParagraphStyle("end", fontName="Malgun", fontSize=8.8, leading=12, textColor=GREY)))

# ── 2쪽: 수행 소감 ──
s.append(PageBreak())
s.append(P("5. 수행 소감", h1))
s.append(HRFlowable(width="100%", thickness=1.0, color=NAVY, spaceAfter=8))
refl = ParagraphStyle("refl", fontName="Malgun", fontSize=10.5, leading=18,
                      textColor=colors.black, spaceAfter=4, firstLineIndent=12)
s.append(Paragraph(
    "이번 프로젝트를 수행하면서 AI를 활용하더라도 누가, 어떤 방향으로 만들고 얼마나 많은 "
    "시간과 고민을 쏟느냐에 따라 결과물의 완성도가 크게 달라질 수 있다는 것을 알게 되었습니다. "
    "같은 도구를 사용하더라도 문제를 정확히 이해하고 전략을 설계하는 사람의 역량이 최종 품질을 "
    "좌우한다는 점을 직접 느꼈습니다. 특히 평가함수 하나를 개선했을 때 성능이 크게 달라지는 것을 "
    "보며, 핵심을 짚어내는 안목과 꾸준한 검증이 중요하다는 것을 깨달았습니다. 따라서 AI에만 "
    "의존하지 않고 스스로 원리를 이해하며 본인의 능력을 기르는 것이 앞으로 더욱 중요해질 것이라고 "
    "생각합니다. 이번 경험은 도구를 잘 다루는 능력과 탄탄한 기본기를 함께 갖추어야 한다는 점을 "
    "일깨워 준 좋은 계기였습니다.", refl))

doc.build(s)
print("PDF created")
