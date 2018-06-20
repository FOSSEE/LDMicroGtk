  0:set bit '$mcr'
  1:# 
  2:# start rung 2
  3:let bit '$rung_top' := '$mcr'
  4:
  5:# start series [
  6:if not 'Rosc' {
  7:    clear bit '$rung_top'
  8:}
  9:
 10:if not '$Tof_antiglitch' {
 11:    let var 'Tof' := 9
 12:}
 13:set bit '$Tof_antiglitch'
 14:if not '$rung_top' {
 15:    if 'Tof' < 9 {
 16:        increment 'Tof'
 17:        set bit '$rung_top'
 18:    }
 19:} else {
 20:    let var 'Tof' := 0
 21:}
 22:
 23:if '$rung_top' {
 24:    if 'Ton' < 9 {
 25:        increment 'Ton'
 26:        clear bit '$rung_top'
 27:    }
 28:} else {
 29:    let var 'Ton' := 0
 30:}
 31:
 32:if '$rung_top' {
 33:    clear bit 'Rosc'
 34:} else {
 35:    set bit 'Rosc'
 36:}
 37:
 38:# ] finish series
 39:# 
 40:# start rung 3
 41:let bit '$rung_top' := '$mcr'
 42:
 43:# start series [
 44:if not 'Rosc' {
 45:    clear bit '$rung_top'
 46:}
 47:
 48:if not 'Xup' {
 49:    clear bit '$rung_top'
 50:}
 51:
 52:if '$rung_top' {
 53:    if not '$oneShot_0000' {
 54:        increment 'Ccnt'
 55:    }
 56:}
 57:let bit '$oneShot_0000' := '$rung_top'
 58:if 'Ccnt' < 20 {
 59:    clear bit '$rung_top'
 60:} else {
 61:    set bit '$rung_top'
 62:}
 63:
 64:if 'Trto' < 999 {
 65:    if '$rung_top' {
 66:        increment 'Trto'
 67:    }
 68:    clear bit '$rung_top'
 69:} else {
 70:    set bit '$rung_top'
 71:}
 72:
 73:let bit 'Yup' := '$rung_top'
 74:
 75:# ] finish series
 76:# 
 77:# start rung 4
 78:let bit '$rung_top' := '$mcr'
 79:
 80:# start series [
 81:if not 'Rosc' {
 82:    clear bit '$rung_top'
 83:}
 84:
 85:if not 'Xdown' {
 86:    clear bit '$rung_top'
 87:}
 88:
 89:if '$rung_top' {
 90:    if not '$oneShot_0001' {
 91:        let var '$scratch' := 1
 92:        let var 'Ccnt' := 'Ccnt' - '$scratch'
 93:    }
 94:}
 95:let bit '$oneShot_0001' := '$rung_top'
 96:if 'Ccnt' < 10 {
 97:    clear bit '$rung_top'
 98:} else {
 99:    set bit '$rung_top'
100:}
101:
102:let bit 'Ydown' := '$rung_top'
103:
104:# ] finish series
105:# 
106:# start rung 5
107:let bit '$rung_top' := '$mcr'
108:
109:# start series [
110:if not 'Xres' {
111:    clear bit '$rung_top'
112:}
113:
114:# start parallel [
115:clear bit '$parOut_0000'
116:let bit '$parThis_0000' := '$rung_top'
117:let bit '$scratch' := '$parThis_0000'
118:if '$oneShot_0002' {
119:    clear bit '$parThis_0000'
120:}
121:let bit '$oneShot_0002' := '$scratch'
122:
123:if '$parThis_0000' {
124:    set bit '$parOut_0000'
125:}
126:let bit '$parThis_0000' := '$rung_top'
127:let bit '$scratch' := '$parThis_0000'
128:if not '$parThis_0000' {
129:    if '$oneShot_0003' {
130:        set bit '$parThis_0000'
131:    }
132:} else {
133:    clear bit '$parThis_0000'
134:}
135:let bit '$oneShot_0003' := '$scratch'
136:
137:if '$parThis_0000' {
138:    set bit '$parOut_0000'
139:}
140:let bit '$rung_top' := '$parOut_0000'
141:# ] finish parallel
142:if '$rung_top' {
143:    let var 'Trto' := 0
144:}
145:
146:# ] finish series
147:# 
148:# start rung 6
149:let bit '$rung_top' := '$mcr'
150:
151:# start series [
152:if not 'Rosc' {
153:    clear bit '$rung_top'
154:}
155:
156:if '$rung_top' {
157:    if not '$oneShot_0004' {
158:        increment 'Ccirc'
159:        if 'Ccirc' < 8 {
160:        } else {
161:            let var 'Ccirc' := 0
162:        }
163:    }
164:}
165:let bit '$oneShot_0004' := '$rung_top'
166:
167:# ] finish series
168:# 
169:# start rung 7
170:let bit '$rung_top' := '$mcr'
171:
172:# start series [
173:let var '$scratch2' := 3
174:if 'Ccirc' == '$scratch2' {
175:} else {
176:    clear bit '$rung_top'
177:}
178:
179:let bit '$scratch' := '$rung_top'
180:if not '$rung_top' {
181:    if '$oneShot_0005' {
182:        set bit '$rung_top'
183:    }
184:} else {
185:    clear bit '$rung_top'
186:}
187:let bit '$oneShot_0005' := '$scratch'
188:
189:if not '$Tpulse_antiglitch' {
190:    let var 'Tpulse' := 19
191:}
192:set bit '$Tpulse_antiglitch'
193:if not '$rung_top' {
194:    if 'Tpulse' < 19 {
195:        increment 'Tpulse'
196:        set bit '$rung_top'
197:    }
198:} else {
199:    let var 'Tpulse' := 0
200:}
201:
202:let bit 'Ypulse' := '$rung_top'
203:
204:# ] finish series
