from flask import (
    Flask,
    render_template,
    request,
    redirect,
    url_for,
    flash,
    session,
    jsonify,
)
from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash
import OTP_send


app = Flask(__name__)
app.secret_key = "Barf_Ka_Gola"

# Database Configuration
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///users.db"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
db = SQLAlchemy(app)


# User Model
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(100), unique=True, nullable=False)
    phone = db.Column(db.String(10), unique=True, nullable=False)
    email = db.Column(db.String(100), unique=True, nullable=False)
    password_hash = db.Column(db.String(200), nullable=False)


# Initialize Database within Application Context
with app.app_context():
    db.create_all()


@app.route("/")
def index():
    return render_template("login.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        username = request.form["username"]
        phone = request.form["phone_number"]
        email = request.form["email"]
        password = request.form["password"]

        hashed_password = generate_password_hash(password, method="pbkdf2:sha256")
        new_user = User(
            username=username, phone=phone, email=email, password_hash=hashed_password
        )
        db.session.add(new_user)
        db.session.commit()

        flash("Registration successful! Please login.")
        return redirect(url_for("index"))

    return render_template("register.html")


@app.route("/login", methods=["POST"])
def login():
    username = request.form["username"]
    password = request.form["password"]
    user = User.query.filter_by(username=username).first()

    if user and check_password_hash(user.password_hash, password):
        session["user_id"] = user.id
        session["username"] = user.username
        return redirect(url_for("dashboard"))
    else:
        flash("Invalid username or password", "error")
        return redirect(url_for("index"))


@app.route("/dashboard")
def dashboard():
    if "user_id" in session:
        return render_template("dashboard.html", username=session["username"])
    else:
        return redirect(url_for("index"))


@app.route("/logout")
def logout():
    session.pop("user_id", None)
    session.pop("username", None)
    return redirect(url_for("index"))


@app.route("/forgot-password")
def forgot_pass():
    return render_template("forgot_pass.html")


@app.route("/check-username", methods=["POST"])
def check_username():
    username = request.form["username"]
    user = User.query.filter_by(username=username).first()
    ans = 1
    if len(username) < 4 or len(username) > 30:
        ans = 0
    if user or (ans == 0):
        return jsonify({"available": False})
    else:
        return jsonify({"available": True})


@app.route("/check-phone", methods=["POST"])
def check_phone():
    phone = request.form["phone_number"]
    user = User.query.filter_by(phone=phone).first()
    if user:
        return jsonify({"available": False})
    else:
        return jsonify({"available": True})


@app.route("/check-email", methods=["POST"])
def check_email():
    email = request.form["email"]
    user = User.query.filter_by(email=email).first()
    if user:
        return jsonify({"available": False})
    else:
        return jsonify({"available": True})


@app.route("/second_page")
def second_page():
    return render_template("graph.html")


if __name__ == "__main__":
    app.run(debug=True)
