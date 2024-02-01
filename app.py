from flask import (
    Flask,
    render_template,
    request,
    redirect,
    url_for,
    session,
    jsonify,
)

def split_list(s):
    data_list = s.split(',')
    return data_list
from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash
import OTP_send, match, company_data
import re, random
import info_stocks_index
import filter
from linegraph import create_plot,   update_plot, save_plotly_figure

app = Flask(__name__)
app.secret_key = "Barf_Ka_Gola"

app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///users.db"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
db = SQLAlchemy(app)


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(100), unique=True, nullable=False)
    phone = db.Column(db.String(10), unique=True, nullable=False)
    email = db.Column(db.String(100), unique=True, nullable=False)
    password_hash = db.Column(db.String(200), nullable=False)


with app.app_context():
    db.create_all()


@app.route("/")
def index():
    return render_template("dashboard.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        username = request.form["username"]
        phone = request.form["phone_number"]
        email = request.form["email"]
        password = request.form["password"]

        otp = "".join(random.choices("0123456789", k=6))
        subject = "Infty Signup:- E-mail Verification"
        message = (
            otp + " is your OTP for username " + username + " to register for Infty."
        )
        OTP_send.send_email(email, subject, message)

        session["otp"] = otp
        session["username"] = username
        session["phone"] = phone
        session["email"] = email
        session["password"] = password

        return redirect(url_for("verify_otp"))

    return render_template("register.html")


@app.route("/verify-otp", methods=["GET", "POST"])
def verify_otp():
    if request.method == "POST":
        entered_otp = (
            request.form["a"]
            + request.form["b"]
            + request.form["c"]
            + request.form["d"]
            + request.form["e"]
            + request.form["f"]
        )
        if session.get("otp") == entered_otp:
            username = session.pop("username")
            phone = session.pop("phone")
            email = session.pop("email")
            password = session.pop("password")

            hashed_password = generate_password_hash(password, method="pbkdf2:sha256")
            new_user = User(
                username=username,
                phone=phone,
                email=email,
                password_hash=hashed_password,
            )
            db.session.add(new_user)
            db.session.commit()

            return render_template(
                "login.html", message="Registration successful! Please login."
            )
        else:
            return render_template(
                "verify_otp.html", message="Invalid OTP. Please try again."
            )

    return render_template("verify_otp.html")


@app.route("/resend-otp", methods=["GET", "POST"])
def resend_otp():
    otp = "".join(random.choices("0123456789", k=6))
    subject = "Infty Signup:- E-mail Verification"
    message = (
        otp
        + " is your OTP for username "
        + session["username"]
        + " to register for Infty."
    )
    OTP_send.send_email(session["email"], subject, message)
    session["otp"] = otp
    return redirect(url_for("verify_otp"))


@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form["username"]
        password = request.form["password"]
        user = User.query.filter_by(username=username).first()

        if user and check_password_hash(user.password_hash, password):
            session["user_id"] = user.id
            session["username"] = user.username
            return redirect(url_for("dashboard"))
        else:
            return render_template(
                "login.html", message="Wrong credentials, please try again."
            )

    return render_template("login.html")


@app.route("/dashboard")
def dashboard():
    if "user_id" in session:
        return render_template("dashboard.html", username=session["username"])
    else:
        return redirect(url_for("index"))


@app.route("/forgot-password", methods=["POST", "GET"])
def forgot_pass():
    if request.method == "POST":
        email = request.form["email"]
        user = User.query.filter_by(email=email).first()
        username = user.username
        otp = "".join(random.choices("0123456789", k=6))
        subject = "Infty Password change:- E-Mail Verification"
        message = (
            otp + " is your OTP for username " + username + " to reset the password."
        )
        OTP_send.send_email(email, subject, message)

        session["otp"] = otp
        session["email"] = email
        session["username"] = username

        return redirect(url_for("verify_otp_forgot"))

    return render_template("forgot_pass.html")


@app.route("/verify-otp-forgot", methods=["GET", "POST"])
def verify_otp_forgot():
    if request.method == "POST":
        entered_otp = (
            request.form["a"]
            + request.form["b"]
            + request.form["c"]
            + request.form["d"]
            + request.form["e"]
            + request.form["f"]
        )
        if session["otp"] == entered_otp:
            return redirect(url_for("new_password"))
        else:
            return render_template(
                "change_password.html", message="Invalid OTP. Please try again."
            )

    return render_template("change_password.html")


@app.route("/resend-otp-forgot", methods=["GET", "POST"])
def resend_otp_forgot():
    otp = "".join(random.choices("0123456789", k=6))
    subject = "Infty Password change:- E-Mail Verification"
    message = (
        otp
        + " is your OTP for username "
        + session["username"]
        + " to reset the password."
    )
    OTP_send.send_email(session["email"], subject, message)

    session["otp"] = otp

    return redirect(url_for("verify_otp_forgot"))


@app.route("/new-password", methods=["GET", "POST"])
def new_password():
    if request.method == "POST":
        email = session.pop("email")
        new_password = request.form["password"]

        user = User.query.filter_by(email=email).first()

        hashed_password = generate_password_hash(new_password, method="pbkdf2:sha256")
        user.password_hash = hashed_password

        db.session.commit()
        return render_template(
            "login.html", message="Password changed successfully. Please login"
        )

    return render_template("new_password.html")


@app.route("/check-email-forgot", methods=["POST"])
def check_email_forgot():
    email = request.form["email"]
    user = User.query.filter_by(email=email).first()

    if user:
        return jsonify({"exist": True})
    else:
        return jsonify({"exist": False})


@app.route("/check-username", methods=["POST"])
def check_username():
    username = request.form["username"]
    user = User.query.filter_by(username=username).first()
    if len(username) < 4:
        return jsonify({"available": False, "reason": "short"})
    elif len(username) > 30:
        return jsonify({"available": False, "reason": "long"})
    elif user:
        return jsonify({"available": False, "reason": "exist"})
    else:
        return jsonify({"available": True})


@app.route("/check-phone", methods=["POST"])
def check_phone():
    phone = request.form["phone_number"]
    user = User.query.filter_by(phone=phone).first()

    if (not (phone.isdigit())) or len(phone) != 10:
        return jsonify({"available": False, "reason": "invalid"})
    elif user:
        return jsonify({"available": False, "reason": "exist"})
    else:
        return jsonify({"available": True})


@app.route("/check-email", methods=["POST"])
def check_email():
    email = request.form["email"]
    user = User.query.filter_by(email=email).first()
    pattern = r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$"

    if bool(re.match(pattern, email)) == 0:
        return jsonify({"available": False, "reason": "invalid"})
    elif user:
        return jsonify({"available": False, "reason": "exist"})
    else:
        return jsonify({"available": True})


@app.route("/check-password1", methods=["POST"])
def check_password():
    password = request.form["password"]
    ans = True

    if len(password) < 8:
        ans = False
    if not re.search(r"[A-Z]", password):
        ans = False
    if not re.search(r"[a-z]", password):
        ans = False
    if not re.search(r"\d", password):
        ans = False
    if not re.search(r'[!@#$%^&*(),.?":{}|<>]', password):
        ans = False
    if ans == False:
        return jsonify({"available": False})
    else:
        return jsonify({"available": True})


@app.route("/second_page")
def second_page():
    return render_template("graph.html")


@app.route("/about")
def about():
    return render_template("about.html")


@app.route("/contact")
def contact():
    return render_template("contact.html")


@app.route("/help")
def help():
    return render_template("help.html")


@app.route("/logout")
def logout():
    session.pop("user_id", None)
    session.pop("username", None)
    return redirect(url_for("index"))


@app.route("/get-profile-info", methods=["POST"])
def get_info():
    username = session["username"]
    user = User.query.filter_by(username=username).first()
    email = user.email
    return jsonify({"username": username, "email": email})


@app.route("/search-stock", methods=["POST"])
def search_stock():
    search = request.form["search"]
    ans = match.top_4_stocks(search)
    return jsonify(
        {
            "sym1": ans[0][0],
            "com1": ans[0][1],
            "sym2": ans[1][0],
            "com2": ans[1][1],
            "sym3": ans[2][0],
            "com3": ans[2][1],
            "sym4": ans[3][0],
            "com4": ans[3][1],
        }
    )


@app.route("/search-all", methods=["POST"])
def search_all():
    search = request.form["search"]
    ans = match.top_4_stocks(search)
    return jsonify(
        {
            "sym1": ans[0][0],
            "com1": ans[0][1],
            "sym2": ans[1][0],
            "com2": ans[1][1],
            "sym3": ans[2][0],
            "com3": ans[2][1],
            "sym4": ans[3][0],
            "com4": ans[3][1],
        }
    )


@app.route("/indices_<index_name>", methods=["GET", "POST"])
def index_data(index_name):
    tagline = {
        "NIFTY 50": "nifty fifty",
        "NIFTY 100": "nifty hundred",
        "NIFTY NEXT 50": "nifty next",
        "NIFTY BANK": "nifty bank",
    }
    session["indexname"] = index_name
    return render_template(
        "indices.html", index_name=index_name, tagline=tagline[index_name]
    )


@app.route("/send_index_data")
def get_data_index():
    symbols = {
        "NIFTY 50": "^NSEI",
        "NIFTY 100": "^CNX100",
        "NIFTY NEXT 50": "^NSMIDCP",
        "NIFTY BANK": "^NSEBANK",
    }
    data = info_stocks_index.market_info_index(symbols[session["indexname"]])
    print(data)
    return jsonify({"data": data})


@app.route("/stock_<stock_sym>", methods=["POST", "GET"])
def stock_data(stock_sym):
    session["current_symbol"] = stock_sym
    company = company_data.company_data[stock_sym]
    return render_template("stocks.html", company=company, sym=stock_sym)


@app.route("/filter", methods = ["POST", "GET"])
def find_filter():
    session["quantity"] = request.form["quantity"]
    session["max"] = request.form["max"]
    session["min"] = request.form["min"]
    
    print(session["quantity"])
    print(session["max"])
    print(session["min"])
    return jsonify({})
    
def get_data():
    data = info_stocks_index.market_info(session["current_symbol"] + ".NS")
    print(data)
    return jsonify({"data": data})

@app.route("/send_filter_data")
def get_filter_data():
    
    if(session['max'] == "" and session["min"] == ""):
        data = ["Hello from here"]
    elif(session['max'] == ""):
        data = filter.filter_stocks_min(int(session["min"]), session["quantity"])
    elif(session['min'] == ""):
        data = filter.filter_stocks_max(int(session["max"]), session["quantity"])
    else:
        data = filter.filter_stocks(int(session["max"]), int(session["max"]), session["quantity"])
    print(data)
    return jsonify({"data": data})

@app.route("/send_graph", methods = ["GET","POST"])
def send_graph():
    session["graph_values"] = request.form['x']
    print(session["graph_values"] )
    return jsonify({})

@app.route("/graph_page", methods = ["GET", "POST"])
def open_stock_page():
    array_symbol = split_list(session["graph_values"])
    if(array_symbol[0] == "Single Stock"):
        ticker_symbol = array_symbol[2] + ".NS"
        period = array_symbol[3]
        fig = create_plot(ticker_symbol, period)
        
        save_plotly_figure(fig)

        with open('figure.html', 'r',encoding='utf-8') as file:
            plotly_figure_html = file.read()

        return render_template("dashboard.html", plotly_figure_html=plotly_figure_html)
    
if __name__ == "__main__":
    app.run(debug=True)
